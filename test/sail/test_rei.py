import unittest
from rmtest import ModuleTestCase
import os

module_path = os.environ['REDIS_MODULE_PATH']
redis_path = os.environ['REDIS_SERVER_PATH']
vw_path = os.environ['VW_BUILD_PATH']


class SAILReiTestCase(ModuleTestCase(module_path, redis_path)):
    def test_rei_new_should_create_model(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.rei.new',
                                                '*rei-models*',
                                                'm0',
                                                '-b 28 -l 0.1',
                                                '*m0-memory*'))
            self.assertExists(r, 'm0')

    def test_rei_new_should_register_model(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.rei.new',
                                                '*rei-models*',
                                                'm0',
                                                '-b 28 -l 0.1 --quiet',
                                                '*m0-memory*'))
            self.assertEqual(r.execute_command('hexists', '*rei-models*', 'm0'),
                             1)
            self.assertEqual(r.execute_command('hget', '*rei-models*', 'm0'),
                             '*m0-memory*')


if __name__ == '__main__':
    unittest.main()

