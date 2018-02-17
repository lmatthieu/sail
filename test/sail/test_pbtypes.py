import unittest
from rmtest import ModuleTestCase
import os
import redis

module_path = os.environ['REDIS_MODULE_PATH']
redis_path = os.environ['REDIS_SERVER_PATH']
vw_path = os.environ['VW_BUILD_PATH']


class SAILPbTypesTests(ModuleTestCase(module_path, redis_path)):

    def initialize(self, type_name, type_key="*sail-key*"):
        self.type_name = type_name
        self.type_key = type_key

    def setUp(self):
        self.initialize("SAIL.VW.VOWPALMODEL")

    def test_new_should_create_valid_object(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('%s.NEW' %self.type_name,
                                                self.type_key))
            self.assertExists(r, self.type_key)

    def test_rdb_should_save_object(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('%s.NEW' %self.type_name,
                                                self.type_key))
            self.assertExists(r, self.type_key)
            r.retry_with_rdb_reload()
            self.assertExists(r, self.type_key)
            #self.assertEqual(r.execute_command('%s.PRINT' %self.type_name,
            #                                   self.type_key, "P"), "")


class SAILPbTypes_VW_VOWPALMETADATA(SAILPbTypesTests):
    def setUp(self):
        self.initialize("SAIL.VW.VOWPALMETADATA")


if __name__ == '__main__':
    unittest.main()
