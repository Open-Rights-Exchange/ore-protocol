import unittest
from eosfactory.eosf import *

verbosity([Verbosity.INFO, Verbosity.OUT, Verbosity.TRACE, Verbosity.DEBUG])

STANDARD_TOKEN = "~/Workspace/testeos/contracts/ore.standard_token/"
RIGHTS_REGISTRY = "~/Workspace/ore-protocol/contracts/ore.rights_registry/"
INSTRUMENT = "~/Workspace/ore-protocol/contracts/ore.instrument/"
USAGE_LOG = "~/Workspace/ore-protocol/contracts/ore.usage_log/"
APIM_MANAGER = "~/Workspace/ore-protocol/contracts/apim.manager/"

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
        create_account("token", master)
        token_contract = Contract(token, "ore.standard_token")
        token_contract.build()
        token_contract.deploy()

        create_account("right", master)
        right_contract = Contract(right, "ore.rights_registry")
        right_contract.build()
        right_contract.deploy()

        create_account("instr", master)
        instr_contract = Contract(instr, "ore.instrument")
        instr_contract.build()
        instr_contract.deploy()

        create_account("apim", master)
        apim_contract = Contract(apim, "apim.manager")
        apim_contract.build()
        apim_contract.deploy()

        create_account("usage", master)
        usage_contract = Contract(usage, "ore.usage_log")
        usage_contract.build()
        usage_contract.deploy()

        # token_contract.delete()
        # right_contract.delete()
        # instr_contract.delete()
        # apim_contract.delete()
        # usage_contract.delete()

    def test_02(self):
        COMMENT('''
        Create and issue tokens:
        ''')

        token.push_action(
                "create",
                {
                    "issuer": token,
                    "maximum_supply": "100000000.0000 CPU"
                },
                permission=(token, Permission.ACTIVE))

        token.push_action(
                "create",
                {
                    "issuer": token,
                    "maximum_supply": "100000000.0000 ORE"
                },
                permission=(token, Permission.ACTIVE))

        instr.push_action(
                "create",
                {
                    "issuer": instr,
                    "maximum_supply": "100000000.0000 OREINST"
                },
                permission=(instr, Permission.ACTIVE))

        token.push_action(
                "issue",
                {
                    "to": app,
                    "quantity": "10000000.0000 CPU",
                    "memo": ""
                },
                permission=(token, Permission.ACTIVE))

        token.push_action(
                "issue",
                {
                    "to": app,
                    "quantity": "10000000.0000 ORE",
                    "memo": ""
                },
                permission=(token, Permission.ACTIVE))

        instr.push_action(
                "issue",
                {
                    "to": instr,
                    "quantity": "10000000.0000 OREINST",
                    "memo": ""
                },
                permission=(instr, Permission.ACTIVE))

    def test_03(self):
        COMMENT('''
        Register rights:
        ''')

        right.push_action(
                "upsertright",
                {
                    "owner": apim,
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
                permission=(apim, Permission.ACTIVE))

        

    def test_04(self):
        COMMENT('''
        Publish and license offers:
        ''')
        with self.assertRaises(Error):
            apim.push_action(
                    "publishapi",
                    {
                        "creator":app,
                        "issuer":app,
                        "api_voucher_license_price_in_cpu":"0",
                        "api_voucher_lifetime_in_seconds": "10",
                        "api_voucher_start_date": "0",
                        "api_voucher_end_date": "0",
                        "api_voucher_mutability": "0",
                        "api_voucher_security_type":"pass",
                        "api_voucher_valid_forever": "0",
                        "right_params": [
                            {
                                "right_name": "apimarket.manager.licenseApi",
                                "right_description":"description",
                                "right_price_in_cpu":"100",
                                "api_name":"apis",
                                "api_description":"desci",
                                "api_price_in_cpu":"10",
                                "api_payment_model":"pass",
                                "api_additional_url_params":""
                            }
                        ],
                        "api_voucher_parameter_rules":[],
                        "offer_mutability": 2,
                        "offer_security_type": "sec",
                        "offer_template":"",
                        "offer_start_time": 0,
                        "offer_end_time":0,
                        "offer_override_id":0 
                    },
                    permission=(app, Permission.ACTIVE))


    def test_05(self):
        COMMENT('''
        Check tables:
        ''')

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
