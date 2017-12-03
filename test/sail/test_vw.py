import unittest
from rmtest import ModuleTestCase
import os, time

module_path = os.environ['REDIS_MODULE_PATH']
redis_path = os.environ['REDIS_SERVER_PATH']
vw_path = os.environ['VW_BUILD_PATH']


class SAILCoreTestCase(ModuleTestCase(module_path, redis_path)):
    def testCreate(self):
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.vw.new', 'm0',
                                                '-b 28 -l 0.1 --quiet'))
            self.assertExists(r, 'm0')

    def testVwSave(self):
        params = '-b 28 -l 0.1 --quiet'
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.vw.new', 'm0', params))
            self.assertExists(r, 'm0')
            r.retry_with_rdb_reload()
            self.assertExists(r, 'm0')
            self.assertEqual(r.execute_command('sail.vw.get', 'm0'), params)

    def testVwRDBLoadSave(self):
        self.assertTrue(self.server)
        self.assertTrue(self.client)
        params = '-b 28 -l 0.2 --quiet'
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.vw.new', 'm0', params))
            self.assertExists(r, 'm0')
            self.assertEqual(r.execute_command('save'), 'OK')
            self.assertEqual(r.execute_command('DEBUG', 'RELOAD'), 'OK')
            self.assertExists(r, 'm0')
            self.assertEqual(r.execute_command('sail.vw.get', 'm0'), params)

    def testVwRewriteAOF(self):
        self.assertTrue(self.server)
        self.assertTrue(self.client)
        params = '-b 28 -l 0.2 --quiet'
        with self.redis() as r:
            self.assertIsNone(r.execute_command('sail.vw.new', 'm0', params))
            self.assertExists(r, 'm0')
            self.restart_and_reload()
            self.assertExists(r, 'm0')
            self.assertEqual(r.execute_command('sail.vw.get', 'm0'), params)



    def testVwLearn(self):
        self.assertTrue(self.server)
        self.assertTrue(self.client)
        params = "-k -l 20 --initial_t 128000 --power_t 1 -c --passes 8 " \
                 "--invariant --ngram 3 --skips 1 --holdout_off"
        with self.redis() as r:
            r.execute_command('sail.vw.new', 'm0', params)
            self.assertExists(r, "m0")
            file_name = "{path}/test/test-sets/0001.dat".format(path=vw_path)
            examples = []
            with open(file_name) as fp:
                for line in fp:
                    examples.append(line)
            # learn
            for ex in examples:
                r.execute_command('sail.vw.fit', 'm0', ex)
            # predict
            predictions = []
            for ex in examples:
                predictions.append(
                    r.execute_command("sail.vw.predict", 'm0', ex))
            self.assertExists(r, "m0")
            self.restart_and_reload()
            # test
            self.assertExists(r, "m0")
            for ex, pred in zip(examples, predictions):
                new_pred = r.execute_command('sail.vw.predict', 'm0', ex)
                self.assertTrue(pred == new_pred)

if __name__ == '__main__':
    unittest.main()
