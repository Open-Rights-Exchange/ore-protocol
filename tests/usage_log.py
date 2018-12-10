import unittest
from eosfactory.eosf import *

verbosity([Verbosity.INFO, Verbosity.OUT, Verbosity.TRACE, Verbosity.DEBUG])

class Test(unittest.TestCase):

    def run(self, result=None):
        super().run(result)


    @classmethod
    def setUpClass(cls):
        SCENARIO('''
        Create a contract from template, then build and deploy it.
        ''')
        reset()
        create_wallet()
        create_master_account("master")

        COMMENT('''
        Create test accounts:
        ''')
        create_account("app", master)


    def setUp(self):
        pass


    def test_01(self):
        COMMENT('''
        Create, build and deploy the contracts:
        ''')

        create_account("usage", master)
        usage_contract = Contract(usage, "/root/ore-protocol/contracts/ore.usage_log")
        usage_contract.build()
        usage_contract.deploy()

    def test_02(self):
        COMMENT('''
        Create Log:
        ''')

        usage.push_action(
                "createlog",
                {
                    "instrument_id": 1,
                    "right_name": "right1",
                    "token_hash": "tokenhash1",
                    "timestamp": 100

                },
                permission=(app, Permission.ACTIVE))

        usage.push_action(
                "createlog",
                {
                    "instrument_id": 2,
                    "right_name": "right2",
                    "token_hash": "tokenhash2",
                    "timestamp": 100

                },
                permission=(app, Permission.ACTIVE))

    def test_03(self):
        COMMENT('''
        Delete Log:
        ''')
        usage.push_action(
                "deletelog",
                {
                    "instrument_id": 2,
                    "token_hash": "tokenhash2"

                },
                permission=(app, Permission.ACTIVE))
        
        with self.assertRaises(Error): #No log exist for the given pair or right and instrument
            usage.push_action(
                    "deletelog",
                    {
                        "instrument_id": 2,
                        "token_hash": "tokenhash2"

                    },
                    permission=(app, Permission.ACTIVE))

    def test_04(self):
        COMMENT('''
        Update Count:
        ''')
        
        usage.push_action(
                "updatecount",
                {
                    "instrument_id": 1,
                    "right_name": "right1",
                    "cpu": "1.0000 CPU"

                },
                permission=(app, Permission.ACTIVE))

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
