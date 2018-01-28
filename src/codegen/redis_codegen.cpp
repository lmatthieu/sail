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
                   "extern RedisModuleType *$name$Type;\n\n");
    GenerateRdbLoad(printer);
    GenerateRdbSave(printer);
    GenerateAofRewrite(printer);
    GenerateMemUsage(printer);
    GenerateFree(printer);
    GenerateNew(printer);
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
   * Creates an object allocation function
   * - From proto binary format (default)
   * - From proto text format (last arg is TEXT)
   * - From proto json format (last arg is JSON)
   *
   * @param printer
   */
  void GenerateNew(io::Printer *printer) const {

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
    vars_["name"] = service_->name();
    vars_["full_name"] = service_->full_name();
  }

  void GenerateService(io::Printer *printer) {
    printer->Print(vars_, "class $name$ {\n");
    printer->Indent();
    printer->Print(vars_,
                   "protected:\n"
                       "inline $name$() {};\n"
                       "public:\n"
                       "virtual ~$name$();\n");
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
                     "virtual void $name$(sail::Context* context,\n"
                         "         const $input_type$* request,\n"
                         "         $output_type$* response) {}\n");
    }
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

    io::Printer printer(output.get(), '$');

    // Any include file and string utils
    printer.Print("#include <google/protobuf/any.h>\n"
                      "#include <google/protobuf/stubs/strutil.h>\n");

    // generates the include files
    for (int i = 0; i < file->public_dependency_count(); ++i) {
      auto dep = file->public_dependency(i);
      const char *ext = ".pb.h";
      string dependency = cpp::StripProto(dep->name()) + ext;

      printer.Print(
          "#include \"$dependency$\"\n",
          "dependency", dependency);
    }

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
    }

    for (int i = 0; i < file->message_type_count(); ++i) {
      auto message = file->message_type(i);
      MessageGenerator msg_gen(message);

      msg_gen.AddInclude(&printer);
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

      service_gen.GenerateService(&printer);
    }

    return true;
  }

  virtual ~RedisCodeGenerator() {

  }
};

int main(int argc, char **argv) {
  RedisCodeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
