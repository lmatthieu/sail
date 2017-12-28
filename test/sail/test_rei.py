import unittest
from rmtest import ModuleTestCase
import os
import redis

module_path = os.environ['REDIS_MODULE_PATH']
redis_path = os.environ['REDIS_SERVER_PATH']
vw_path = os.environ['VW_BUILD_PATH']


class SAILReiTestCase(ModuleTestCase(module_path, redis_path)):
    MODEL_REPO = "*rei-m0*"
    MODEL_PARAMS = '-b 28 -l 0.1 --quiet'

    def test_rei_new_should_create_metadata(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.rei.new',
                                                self.MODEL_REPO,
                                                self.MODEL_PARAMS))
            self.assertExists(r, self.MODEL_REPO)
            self.assertEqual(r.execute_command('hmget', self.MODEL_REPO,
                                               "model", "memory",
                                               "parameters",
                                               "eventid"),
                             [self.MODEL_REPO + ":model",
                              self.MODEL_REPO + ":memory",
                              self.MODEL_PARAMS,
                              '0'])

    def test_rei_new_should_create_model(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.rei.new',
                                                self.MODEL_REPO,
                                                self.MODEL_PARAMS))
            model_name = r.execute_command('hget', self.MODEL_REPO, 'model')
            self.assertExists(r, model_name)

    def test_rei_new_should_throw_error_with_key_conflict(self):
        with self.redis() as r:
            r.execute_command('SET', 'testkey', '1')
            try:
                r.execute_command('sail.rei.new',
                                  'testkey',
                                  self.MODEL_PARAMS)
            except Exception as ex:
                self.assertTrue(ex.message.startswith(
                    'Conflict while creating keys'))

    def test_rei_act_without_repo_should_return_error(self):
        with self.redis() as r:
            with self.assertRaises(redis.ResponseError) as context:
                r.execute_command('sail.rei.act', 'm0', "example")


if __name__ == '__main__':
    unittest.main()
