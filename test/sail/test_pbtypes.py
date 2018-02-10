import unittest
from rmtest import ModuleTestCase
import os
import redis

module_path = os.environ['REDIS_MODULE_PATH']
redis_path = os.environ['REDIS_SERVER_PATH']
vw_path = os.environ['VW_BUILD_PATH']


class SAILPbTypesTests(ModuleTestCase(module_path, redis_path)):
    type_name = "SAIL.VW.VOWPALMODEL"
    type_key = "*sail-key*"

    def test_new_should_create_valid_object(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('%s.NEW' %self.type_name,
                                                self.type_key))
            self.assertExists(r, self.type_key)



if __name__ == '__main__':
    unittest.main()
