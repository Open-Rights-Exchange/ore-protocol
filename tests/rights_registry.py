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

        create_account("right", master)
        right_contract = Contract(right, "/root/ore-protocol/contracts/ore.rights_registry")
        right_contract.build()
        right_contract.deploy()

    def test_02(self):
        COMMENT('''
        Register rights:
        ''')

        right.push_action(
                "upsertright",
                {
                    "owner": app,
                    "right_name": "apimarket.manager.licenseApi",
                    "urls": [{
                        "base_right": "",
                        "url": " ore://manager.apim/action/licenseapi",
                        "method": "post",
                        "matches_params": [{
                            "name": "sla",
                            "value": "default"
                        }],
                        "token_life_span": 100,
                        "is_default": 1
                    }],
                    "issuer_whitelist": ["app.apim"]
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #You are not the issuer of the existing contract. Update canceled.
            right.push_action(
                    "upsertright",
                    {
                        "owner": app,
                        "right_name": "apimarket.manager.licenseApi",
                        "urls": [{
                            "base_right": "",
                            "url": " ore://manager.apim/action/licenseapi",
                            "method": "post",
                            "matches_params": [{
                                "name": "sla",
                                "value": "default"
                            }],
                            "token_life_span": 100,
                            "is_default": 1
                        }],
                        "issuer_whitelist": ["app.apim"]
                    },
                    permission=(app, Permission.ACTIVE))

        right.push_action(
                "upsertright",
                {
                    "owner": app,
                    "right_name": "apimarket.manager.licenseApi2",
                    "urls": [{
                        "base_right": "",
                        "url": " ore://manager.apim/action/licenseapi",
                        "method": "post",
                        "matches_params": [{
                            "name": "sla",
                            "value": "default"
                        }],
                        "token_life_span": 100,
                        "is_default": 1
                    }],
                    "issuer_whitelist": ["app.apim"]
                },
                permission=(app, Permission.ACTIVE))

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
