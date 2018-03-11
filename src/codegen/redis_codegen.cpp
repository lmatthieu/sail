/**
 * SAIL - A shallow/simple AI learning environment
 * Copyright (C) 2017 - 2018 Matthieu Lagacherie
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/compiler/cpp/cpp_file.h>
#include "options.pb.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace google::protobuf::compiler;
using namespace google::protobuf;

class MessageGenerator {
 public:
  explicit MessageGenerator(const Descriptor *message)
      : message_(message) {
    // Processing options
    const MessageOptions &opts = message->options();
    string wrapper_name = opts.GetExtension(wrapper);

    if (wrapper_name.size() == 0) {
      vars_["classname"] = message_->name();
      custom_wrapper_ = false;
    } else {
      vars_["classname"] = wrapper_name;
      custom_wrapper_ = true;
    }
    vars_["name"] = message_->name();
    vars_["full_name"] = message_->full_name();
    vars_["FULL_NAME"] = ToUpper(message_->full_name());
  }

  void AddInclude(io::Printer *printer) {
    if (custom_wrapper_) {
      string inc_name = vars_["classname"];

      LowerString(&inc_name);
      printer->Print("#include \"sail/api/$decl$.h\"\n", "decl", inc_name);
    }
  }

  void Debug(io::Printer *printer) {
  }

  void GenerateRdbLoad(io::Printer *printer) {
    printer->Print(vars_,
                   "void *$name$TypeRdbLoad(RedisModuleIO *rdb, int encver) {\n");
    printer->Indent();

    // Declarations
    printer->Print(vars_,
                   "$classname$ *obj = new $classname$();\n"
                       "size_t size = RedisModule_LoadUnsigned(rdb);\n"
                       "char *cbuf = RedisModule_LoadStringBuffer(rdb, &size);\n\n");


    // Parsing
    printer->Print(vars_,
                   "if (obj->ParseFromString(cbuf)) {\n");

    // Custom deserializer
    if (custom_wrapper_)
      printer->Print(vars_,
                     "  obj->wrapperDeserialize();\n");

    printer->Print(vars_,
                   "  return obj;\n"
                       "} else {\n"
                       "  return NULL;\n"
                       "}\n");

    printer->Outdent();
    printer->Print("}\n\n");
  }

  void GenerateRdbSave(io::Printer *printer) {
    printer->Print(vars_,
                   "void $name$TypeRdbSave(RedisModuleIO *rdb, void *value) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "$classname$ *obj = reinterpret_cast<$classname$ *>(value);\n");
    // Custom serializer
    if (custom_wrapper_)
      printer->Print(vars_,
                     "obj->wrapperSerialize();\n");
    printer->Print(vars_,
                   "const std::string &buf = obj->SerializeAsString();\n\n"
                       "RedisModule_SaveUnsigned(rdb, buf.size());\n"
                       "RedisModule_SaveStringBuffer(rdb, buf.data(), buf.size());\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

  void GenerateAofRewrite(io::Printer *printer) {
    printer->Print(vars_,
                   "void $name$TypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "$classname$ *obj = reinterpret_cast<$classname$ *>(value);\n");
    // Custom serializer
    if (custom_wrapper_)
      printer->Print(vars_,
                     "obj->wrapperSerialize();\n");
    printer->Print(vars_,
                   "const std::string &buf = obj->SerializeAsString();\n\n"
                       "sds obj_s = sdsempty();\n"
                       "sdscatlen(obj_s, buf.data(), buf.size());\n\n");
    printer->Print(vars_,
                   "RedisModule_EmitAOF(aof, \"$FULL_NAME$.NEW\", \"sb\", key, obj_s, sdslen(obj_s));\n");

    printer->Outdent();
    printer->Print("}\n\n");
  }

  void GenerateMessage(io::Printer *printer) {
    Debug(printer);
    printer->Print(vars_,
                   "static RedisModuleType *$name$Type = 0;\n\n");
    GenerateRdbLoad(printer);
    GenerateRdbSave(printer);
    GenerateAofRewrite(printer);
    GenerateMemUsage(printer);
    GenerateFree(printer);
    GenerateNew(printer);
    GeneratePrint(printer);
    GenerateAPIDeclaration(printer);
  }

  void GenerateMemUsage(io::Printer *printer) const {
    printer->Print(vars_,
                   "size_t $name$TypeMemUsage(const void *value) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "const $classname$ *obj = reinterpret_cast<const $classname$ *>(value);\n"
                       "return obj->SpaceUsed();\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

  void GenerateFree(io::Printer *printer) const {
    printer->Print(vars_,
                   "void $name$TypeFree(void *value) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "$classname$ *obj = reinterpret_cast<$classname$ *>(value);\n"
                       "delete obj;\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

  /**
   * Print a message
   * @param printer
   */
  void GeneratePrint(io::Printer *printer) const {
    printer->Print(vars_,
                   "int $name$TypePrint(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "if (argc < 2) {\n"
                       "  return RedisModule_WrongArity(ctx);\n"
                       "}\n");
    printer->Print(vars_,
                   "auto key = reinterpret_cast<RedisModuleKey *>"
                       "(RedisModule_OpenKey(ctx, argv[1], "
                       "REDISMODULE_READ));\n");
    printer->Print(vars_,
                   "int type = RedisModule_KeyType(key);\n\n"
                       "if (type == REDISMODULE_KEYTYPE_EMPTY || "
                       "RedisModule_ModuleTypeGetType(key) != $name$Type) {\n"
                       "  return RedisModule_ReplyWithError(ctx, SAIL_ERROR_EMPTYKEY_OR_WRONGTYPE);\n"
                       "}\n\n");
    printer->Print(vars_,
                   "$classname$ *obj = reinterpret_cast<$classname$ *>(RedisModule_ModuleTypeGetValue(key));\n\n");
    printer->Print(vars_,
                   "if (obj == nullptr) {\n"
                       "  return RedisModule_ReplyWithError(ctx, \"Cannot get value from the specified key\");\n"
                       "}\n");
    printer->Print(vars_, "std::string buf;\n"
        "size_t len = 0;\n"
        "const char *option = nullptr;\n");
    printer->Print(vars_, "if (argc == 3) {\n"
        "  option = RedisModule_StringPtrLen(argv[2], &len);\n"
        "}\n");

    // Testing args
    printer->Print(vars_,
                   "if (argc == 2 || (len > 0 && option[0] == 'P')) {\n"
                       "  buf = obj->ShortDebugString();\n"
                       "} else if (argc == 3 && len > 0 && option[0] == 'J') {");
    printer->Indent();
    printer->Print(vars_,
                   "google::protobuf::util::JsonPrintOptions options;\n"
                       "options.add_whitespace = true;\n"
                       "options.always_print_primitive_fields = true;\n"
                       "options.preserve_proto_field_names = true;\n");
    printer->Print(vars_,
                   "google::protobuf::util::MessageToJsonString(*obj, &buf, options);\n");
    printer->Outdent();
    printer->Print(vars_, "}\n");
    printer->Print(vars_,
                   "RedisModule_ReplyWithStringBuffer(ctx, buf.data(), buf.size());\n");
    printer->Print(vars_, "return REDISMODULE_OK;\n");

    printer->Outdent();
    printer->Print("}\n\n");
  }

  /**
   * Creates an object allocation function
   * - From proto binary format (default)
   * - From proto text format (last arg is T for "TEXT")
   * - From proto json format (last arg is J for "JSON")
   * - Empty (last arg is E "EMPTY")
   *
   * @param printer
   */
  void GenerateNew(io::Printer *printer) const {
    printer->Print(vars_,
                   "int $name$TypeNew(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "if (argc < 3) {\n"
                       "  return RedisModule_WrongArity(ctx);\n"
                       "}\n");
    printer->Print(vars_,
                   "auto key = reinterpret_cast<RedisModuleKey *>"
                       "(RedisModule_OpenKey(ctx, argv[1], "
                       "REDISMODULE_READ | REDISMODULE_WRITE));\n");
    printer->Print(vars_,
                   "int type = RedisModule_KeyType(key);\n"
                       "if (type != REDISMODULE_KEYTYPE_EMPTY && "
                       "RedisModule_ModuleTypeGetType(key) != $name$Type) {\n"
                       "  return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);\n"
                       "}\n");
    printer->Print(vars_, "$classname$ *obj = nullptr;\n\n");

    printer->Print(vars_, "if (argc == 3) {\n");
    printer->Indent();

    // Declarations
    printer->Print(vars_,
                   "obj = new $classname$();\n"
                       "size_t len;\n"
                       "const char *cbuf = RedisModule_StringPtrLen(argv[2], &len);\n\n");

    // Parsing
    printer->Print(vars_,
                   "if (obj->ParseFromString(cbuf)) {\n");

    // Custom deserializer
    if (custom_wrapper_)
      printer->Print(vars_,
                     "  obj->wrapperDeserialize();\n");

    printer->Print(vars_,
                   "} else {\n"
                       "  delete obj;\n"
                       "  obj = nullptr;\n"
                       "}\n");

    printer->Outdent();
    printer->Print("} else if (argc == 4) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "obj = new $classname$();\n"
                       "size_t len;\n"
                       "const char *option = RedisModule_StringPtrLen(argv[3], &len);\n\n");

    printer->Print(vars_, "if (option[0] == 'T') {\n"
        "} else if (option[0] == 'J') {\n"
        "} else if (option[0] == 'E') {\n"
        "  obj = new $classname$();\n"
        "} else {\n"
        "  return RedisModule_ReplyWithError(ctx, \"Invalid option value\");\n"
        "}\n\n");

    printer->Outdent();
    printer->Print("}\n\n");

    // Test if the object is allocated
    printer->Print(vars_, "if (obj == nullptr) {\n"
        "  return RedisModule_ReplyWithError(ctx, \"The $name$ object is not allocated\");\n"
        "}\n\n");

    printer->Print(vars_, "if (type != REDISMODULE_KEYTYPE_EMPTY) {\n"
        "  RedisModule_DeleteKey(key);\n"
        "  key = reinterpret_cast<RedisModuleKey *>"
        "(RedisModule_OpenKey(ctx, argv[1], "
        "REDISMODULE_READ | REDISMODULE_WRITE));\n"
        "}\n");
    printer->Print(vars_,
                   "RedisModule_ModuleTypeSetValue(key, $name$Type, obj);\n"
                       "RedisModule_ReplyWithNull(ctx);\n"
                       "return REDISMODULE_OK;\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

  /**
   * Generates a function for loading the REDIS module type
   * @param printer protobuf printer object
   */
  void GenerateAPIDeclaration(io::Printer *printer) const {
    printer->Print(vars_,
                   "int load$name$Type(RedisModuleCtx *ctx, const char *type_name) {\n");
    printer->Indent();
    printer->Print(vars_,
                   "RedisModuleTypeMethods tm;\n\n"
                       "tm.version = REDISMODULE_TYPE_METHOD_VERSION;\n"
                       "tm.rdb_load = $name$TypeRdbLoad;\n"
                       "tm.rdb_save = $name$TypeRdbSave;\n"
                       "tm.aof_rewrite = $name$TypeAofRewrite;\n"
                       "tm.mem_usage = $name$TypeMemUsage;\n"
                       "tm.free = $name$TypeFree;\n\n");

    printer->Print(vars_,
                   "$name$Type = RedisModule_CreateDataType(ctx, type_name, 0, &tm);\n"
                       "if ($name$Type == NULL) {\n"
                       "  std::cerr << \"Cannot initialize type $name$\" << std::endl;\n"
                       "  return REDISMODULE_ERR;\n"
                       "}\n\n");

    printer->Print(vars_,
                   "RMUtil_RegisterWriteCmd(ctx, \"$FULL_NAME$.NEW\", &$name$TypeNew);\n\n");
    printer->Print(vars_,
                   "RMUtil_RegisterWriteCmd(ctx, \"$FULL_NAME$.PRINT\", &$name$TypePrint);\n\n");

    printer->Print("return REDISMODULE_OK;\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

 private:
  const Descriptor *message_;
  map<string, string> vars_;
  bool custom_wrapper_;
};

class ServiceGenerator {
 public:
  explicit ServiceGenerator(const ServiceDescriptor *service)
      : service_(service) {
    // Processing options
    const ServiceOptions &opts = service->options();
    string wrapper_name = opts.GetExtension(simpl);

    if (wrapper_name.size() == 0) {
      vars_["classname"] = service_->name() + "Impl";
    } else {
      vars_["classname"] = wrapper_name;
    }
    vars_["name"] = service_->name();
    vars_["full_name"] = service_->full_name();

    string inc_name = vars_["classname"];
    LowerString(&inc_name);
    vars_["include"] = inc_name;
  }

  void AddInclude(io::Printer *printer) {
    printer->Print(vars_, "#include \"sail/api/$include$.h\"\n");
  }

  void GenerateService(io::Printer *printer) {
    printer->Print(vars_, "class $name$ {\n");
    printer->Indent();
    printer->Print(vars_,
                   "protected:\n"
                       "inline $name$() {};\n"
                       "public:\n"
                       "virtual ~$name$() {};\n");
    GenerateMethods(printer);

    printer->Print(vars_, "\nprivate:\n"
        "GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($name$);\n");
    printer->Outdent();
    printer->Print("};\n\n");
  }

  void GenerateMethods(io::Printer *printer) {
    for (int i = 0; i < service_->method_count(); i++) {
      const MethodDescriptor *method = service_->method(i);
      std::map<string, string> sub_vars;
      sub_vars["name"] = method->name();
      sub_vars["input_type"] = cpp::ClassName(method->input_type(), true);
      sub_vars["output_type"] = cpp::ClassName(method->output_type(), true);

      printer->Print(sub_vars,
                     "virtual int $name$(sail::RedisContext* context,\n"
                         "         const $input_type$* request,\n"
                         "         $output_type$* response) = 0;\n");
    }
  }

  void GenerateRedisFunctions(io::Printer *printer) {
    for (int i = 0; i < service_->method_count(); i++) {
      const MethodDescriptor *method = service_->method(i);
      std::map<string, string> sub_vars;
      bool custom_wrapper = false;
      sub_vars["service_name"] = vars_["classname"];
      sub_vars["name"] = method->name();
      sub_vars["input_type"] = cpp::ClassName(method->input_type(), true);
      sub_vars["output_type"] = cpp::ClassName(method->output_type(), true);

      const MessageOptions &opts = method->input_type()->options();
      string wrapper_name = opts.GetExtension(wrapper);

      if (wrapper_name.size() == 0) {
        sub_vars["classname"] = method->input_type()->name();
        custom_wrapper = false;
      } else {
        sub_vars["classname"] = wrapper_name;
        custom_wrapper = true;
      }

      printer->Print(sub_vars,
                     "int $service_name$$name$("
                         "RedisModuleCtx *ctx, RedisModuleString **argv, "
                         "int argc) {\n");
      printer->Indent();
      printer->Print(sub_vars,
                     "if (argc < 2) {\n"
                         "  return RedisModule_WrongArity(ctx);\n"
                         "}\n");

      printer->Print(sub_vars, "google::protobuf::Arena arena;\n"
          "$classname$ *obj = google::protobuf::Arena::"
          "CreateMessage<$classname$>(&arena);\n"
          "$output_type$ *out_obj = google::protobuf::Arena::"
          "CreateMessage<$output_type$>(&arena);\n"
          "sail::RedisContext rctx(ctx);\n"
          "$service_name$ srv;\n\n");
      printer->Print(sub_vars,
                     "size_t len;\n"
                         "const char *cbuf = RedisModule_StringPtrLen(argv[1], &len);\n\n");
      // Parsing
      printer->Print(sub_vars,
                     "if (obj->ParseFromString(cbuf)) {\n");

      // Custom deserializer
      if (custom_wrapper)
        printer->Print(sub_vars,
                       "  obj->wrapperDeserialize();\n");
      printer->Print(sub_vars,
                     "  if (srv.$name$(&rctx, obj, out_obj) != 0)\n"
                         "    return REDISMODULE_ERR;\n"
                         "  return REDISMODULE_OK;\n");
      printer->Print(sub_vars,
                     "} else {\n"
                         "  return REDISMODULE_ERR;\n"
                         "}\n");

      printer->Outdent();
      printer->Print("}\n\n");
    }
  }

  /**
   * Generates a function for loading the REDIS service
   * @param printer protobuf printer object
   */
  void GenerateAPIDeclaration(io::Printer *printer) {
    printer->Print(vars_,
                   "int load$name$Service(RedisModuleCtx *ctx) {\n");
    printer->Indent();

    for (int i = 0; i < service_->method_count(); i++) {
      const MethodDescriptor *method = service_->method(i);
      std::map<string, string> sub_vars;
      sub_vars["service_name"] = vars_["classname"];
      sub_vars["name"] = method->name();
      sub_vars["input_type"] = cpp::ClassName(method->input_type(), true);
      sub_vars["output_type"] = cpp::ClassName(method->output_type(), true);

      printer->Print(sub_vars,
                     "RMUtil_RegisterWriteCmd(ctx, \"$service_name$.$name$\", &$service_name$$name$);\n\n");
    }

    printer->Print("return REDISMODULE_OK;\n");
    printer->Outdent();
    printer->Print("}\n\n");
  }

 private:
  const ServiceDescriptor *service_;
  std::map<string, string> vars_;
};

class RedisCodeGenerator : public CodeGenerator {
 public:
  RedisCodeGenerator() {}

  bool Generate(const google::protobuf::FileDescriptor *file,
                const std::string &parameter,
                GeneratorContext *generator_context,
                std::string *error) const override {
    cpp::Options file_options;
    string base_name = cpp::StripProto(file->name());
    cpp::FileGenerator file_generator(file, file_options);
    google::protobuf::scoped_ptr<io::ZeroCopyOutputStream> output(
        generator_context->Open(base_name + "_service.cpp"));
    google::protobuf::scoped_ptr<io::ZeroCopyOutputStream> output_header(
        generator_context->Open(base_name + "_service.h"));

    io::Printer printer(output.get(), '$');
    io::Printer printer_header(output_header.get(), '$');

    // Add defines
    printer_header.Print("#ifndef $name$_hpp__\n#define $name$_hpp__\n",
                         "name", base_name);

    // Any include file and string utils
    printer.Print("#include <google/protobuf/any.h>\n"
                      "#include <google/protobuf/stubs/strutil.h>\n"
                      "#include <google/protobuf/text_format.h>\n"
                      "#include <google/protobuf/util/json_util.h>\n"
                      "#include <google/protobuf/arena.h>\n");



    // generates the include files
    for (int i = 0; i < file->public_dependency_count(); ++i) {
      auto dep = file->public_dependency(i);
      const char *ext = ".pb.h";
      string dependency = cpp::StripProto(dep->name()) + ext;

      printer.Print(
          "#include \"$dependency$\"\n",
          "dependency", dependency);
    }

    printer.Print(
        "#include \"$dependency$_service.h\"\n",
        "dependency", base_name);

    // generates message include files
    for (int i = 0; i < file->dependency_count(); i++) {
      const FileDescriptor *dep = file->dependency(i);
      const char *ext = ".pb.h";
      string dependency = cpp::StripProto(dep->name()) + ext;

      printer.Print(
          "#include \"$dependency$\"\n",
          "dependency", dependency);
    }

    {
      const char *ext = ".pb.h";
      string include_file = cpp::StripProto(file->name()) + ext;

      printer.Print(
          "#include \"sail/api/$dependency$\"\n",
          "dependency", include_file);
      printer.Print(
          "#include \"sail/context/context.h\"\n"
      );

      printer_header.Print(
          "#include \"sail/api/$dependency$\"\n",
          "dependency", include_file);
      printer_header.Print(
          "#include \"sail/context/context.h\"\n"
      );
    }

    for (int i = 0; i < file->message_type_count(); ++i) {
      auto message = file->message_type(i);
      MessageGenerator msg_gen(message);

      msg_gen.AddInclude(&printer);
    }
    for (int i = 0; i < file->service_count(); ++i) {
      auto service = file->service(i);
      ServiceGenerator service_gen(service);

      service_gen.AddInclude(&printer);
    }

    // Redis include
    printer.Print("#include \"$dependency$\"\n", "dependency",
                  "redis_modules_sdk/rmutil/util.h");
    printer.Print(
        "extern \"C\" {\n#include <redis_modules_sdk/rmutil/sds.h>\n}\n");
    printer.Print("\n\n");

    cpp::NamespaceOpener ns(cpp::Namespace(file), &printer);

    // Redis type serde
    for (int i = 0; i < file->message_type_count(); ++i) {
      auto message = file->message_type(i);
      MessageGenerator message_gen(message);

      message_gen.GenerateMessage(&printer);
    }

    // Service code generation
    for (int i = 0; i < file->service_count(); ++i) {
      auto service = file->service(i);
      ServiceGenerator service_gen(service);

      service_gen.GenerateRedisFunctions(&printer);
      service_gen.GenerateAPIDeclaration(&printer);
    }

    {
      cpp::NamespaceOpener ns_header(cpp::Namespace(file), &printer_header);

      // Service code generation
      for (int i = 0; i < file->service_count(); ++i) {
        auto service = file->service(i);
        ServiceGenerator service_gen(service);

        service_gen.GenerateService(&printer_header);
      }
    }

    printer_header.Print("#endif\n");

    return true;
  }

  virtual ~RedisCodeGenerator() {

  }
};

int main(int argc, char **argv) {
  RedisCodeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
