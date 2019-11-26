import unittest
from eosfactory.eosf import *
import time

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
        create_account("notminter", master)


    def setUp(self):
        pass

    def test_01(self):

        create_account("right", master, account_name="rights.ore")
        right_contract = Contract(right, "/root/ore-protocol/contracts/ore.rights_registry")
        right_contract.build()
        right_contract.deploy()

        create_account("instr", master, account_name="instr.ore")
        instr_contract = Contract(instr, "/root/ore-protocol/contracts/ore.instrument")
        instr_contract.build()
        instr_contract.deploy()

    def test_02(self):
        COMMENT('''
        Create and Issue OREINST:
        ''')

        instr.push_action(
                "create",
                {
                    "issuer": instr,
                    "maximum_supply": "100000000.0000 OREINST"
                },
                permission=(instr, Permission.ACTIVE))

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
                    "issuer_whitelist": [app]
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
                    "issuer_whitelist": []
                },
                permission=(app, Permission.ACTIVE))

        right.push_action(
                "upsertright",
                {
                    "owner": app,
                    "right_name": "apimarket.manager.licenseApi3",
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
                    "issuer_whitelist": []
                },
                permission=(app, Permission.ACTIVE))

    def test_04(self):
        COMMENT('''
        Mint:
        ''')

        instr.push_action(
                "mint",
                {
                    "minter": app,
                    "owner": app,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "sample_class",
                        "description": "sample_description",
                        "instrument_template": "sample_template",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        },
                        {
                            "right_name": "apimarket.manager.licenseApi2",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0,
                    "instrumentId": 0
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): # Instrument id exists
            instr.push_action(
                    "mint",
                    {
                        "minter": app,
                        "owner": app,
                        "instrument": {
                            "issuer": app,
                            "instrument_class": "sample_class",
                            "description": "sample_description",
                            "instrument_template": "",
                            "security_type": "",
                            "parameter_rules": [],
                            "rights": [{
                                "right_name": "apimarket.manager.licenseApi",
                                "description": "licenser",
                                "price_in_cpu": "10",
                                "additional_url_params": []
                            }],
                            "parent_instrument_id": 0,
                            "data": [],
                            "encrypted_by": "",
                            "mutability": 2
                        },
                        "start_time": 0,
                        "end_time": 0,
                        "instrumentId": 1
                    },
                    permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): # Owner account doesnt exists
            instr.push_action(
                    "mint",
                    {
                        "minter": app,
                        "owner": "notexists",
                        "instrument": {
                            "issuer": app,
                            "instrument_class": "sample_class",
                            "description": "sample_description",
                            "instrument_template": "",
                            "security_type": "",
                            "parameter_rules": [],
                            "rights": [{
                                "right_name": "apimarket.manager.licenseApi",
                                "description": "licenser",
                                "price_in_cpu": "10",
                                "additional_url_params": []
                            }],
                            "parent_instrument_id": 0,
                            "data": [],
                            "encrypted_by": "",
                            "mutability": 2
                        },
                        "start_time": 0,
                        "end_time": 0,
                        "instrumentId": 0
                    },
                    permission=(app, Permission.ACTIVE))

    def test_05(self):
        COMMENT('''
        Create Instrument:
        ''')

        instr.push_action(
                "createinst",
                {
                    "minter": app,
                    "owner": app,
                    "instrumentId": 2,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "sample_class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0
                },
                permission=(instr, Permission.ACTIVE))

        instr.push_action(
                "createinst",
                {
                    "minter": app,
                    "owner": app,
                    "instrumentId": 5,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "sample_class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0
                },
                permission=(instr, Permission.ACTIVE))
        
        
    def test_06(self):
        COMMENT('''
        Update Instrument:
        ''')
        time.sleep(3)
        instr.push_action(
                "createinst",
                {
                    "minter": app,
                    "owner": app,
                    "instrumentId": 2,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "sample_class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0
                },
                permission=(instr, Permission.ACTIVE))


    def test_07(self):
        COMMENT('''
        Check Rights:
        ''')

        instr.push_action(
                "mint",
                {
                    "minter": app,
                    "owner": app,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi3",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0,
                    "instrumentId": 3
                },
                permission=(app, Permission.ACTIVE))

        instr.push_action(
                "checkright",
                {
                    "minter": app,
                    "issuer": app,
                    "rightname": "apimarket.manager.licenseApi",
                    "deferred_transaction_id": 10
                },
                permission=(instr, Permission.ACTIVE))

        with self.assertRaises(Error): # Right doesn't exist
            instr.push_action(
                "checkright",
                {
                    "minter": app,
                    "issuer": app,
                    "rightname": "nonexistent.right",
                    "deferred_transaction_id": 0
                },
                permission=(instr, Permission.ACTIVE))

        with self.assertRaises(Error): # minter neither owns the right nor whitelisted for the right
            instr.push_action(
                "checkright",
                {
                    "minter": "ownerfails",
                    "issuer": app,
                    "rightname": "apimarket.manager.licenseApi",
                    "deferred_transaction_id": 0
                },
                permission=(instr, Permission.ACTIVE))

        with self.assertRaises(Error): # instrument issuer neither holds the right nor whitelisted for the right
            instr.push_action(
                "checkright",
                {
                    "minter": "minter",
                    "issuer": "issuerfails",
                    "rightname": "apimarket.manager.licenseApi",
                    "deferred_transaction_id": 0
                },
                permission=(instr, Permission.ACTIVE))

    def test_08(self):
        COMMENT('''
        Revoke:
        ''')

        instr.push_action(
                "mint",
                {
                    "minter": app,
                    "owner": app,
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi3",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "start_time": 0,
                    "end_time": 0,
                    "instrumentId": 4
                },
                permission=(app, Permission.ACTIVE))

        instr.push_action(
                "revoke",
                {
                    "revoker": app,
                    "token_id": 5
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #token doesn't exist
            instr.push_action(
                    "revoke",
                    {
                        "revoker": app,
                        "token_id": 70
                    },
                    permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): # The revoker account doesn't have authority to revoke the instrument
                instr.push_action(
                    "revoke",
                    {
                        "revoker": right,
                        "token_id": 4
                    },
                    permission=(right, Permission.ACTIVE))

        with self.assertRaises(Error): # token is already revoked
                instr.push_action(
                    "revoke",
                    {
                        "revoker": app,
                        "token_id": 5
                    },
                    permission=(app, Permission.ACTIVE))

    def test_09(self):
        COMMENT('''
        Update:
        ''')

        instr.push_action(
                "update",
                {
                    "updater": app,
                    "instrument_template": "",
                    "instrument": {
                        "issuer": app,
                        "instrument_class": "sample_class",
                        "description": "sample_description",
                        "instrument_template": "",
                        "security_type": "",
                        "parameter_rules": [],
                        "rights": [{
                            "right_name": "apimarket.manager.licenseApi",
                            "description": "licenser",
                            "price_in_cpu": "10",
                            "additional_url_params": []
                        }],
                        "parent_instrument_id": 0,
                        "data": [],
                        "encrypted_by": "",
                        "mutability": 2
                    },
                    "instrument_id": 2,
                    "start_time": 0,
                    "end_time": 0
                },
                permission=(app, Permission.ACTIVE))
        
        with self.assertRaises(Error): #Updater acccount doesn't have the authority to change start/edd time of the instrument
            instr.push_action(
                    "update",
                    {
                        "updater": right,
                        "instrument_template": "",
                        "instrument": {
                            "issuer": app,
                            "instrument_class": "sample_class",
                            "description": "sample_description",
                            "instrument_template": "",
                            "security_type": "",
                            "parameter_rules": [],
                            "rights": [{
                                "right_name": "apimarket.manager.licenseApi",
                                "description": "licenser",
                                "price_in_cpu": "10",
                                "additional_url_params": []
                            }],
                            "parent_instrument_id": 0,
                            "data": [],
                            "encrypted_by": "",
                            "mutability": 2
                        },
                        "instrument_id": 1,
                        "start_time": 0,
                        "end_time": 0
                    },
                    permission=(right, Permission.ACTIVE))

    def test_10(self):
        COMMENT('''
        Transfer:
        ''')

        instr.push_action(
                "transfer",
                {
                    "sender": app,
                    "to": right,
                    "token_id": 2
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Sender account is not allowed to transfer the instrument
            instr.push_action(
                "transfer",
                {
                    "sender": app,
                    "to": right,
                    "token_id": 2
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Token doesn't exists
            instr.push_action(
                "transfer",
                {
                    "sender": app,
                    "to": right,
                    "token_id": 99
                },
                permission=(app, Permission.ACTIVE))


    # def test_06(self):
    # def test_07(self):
    # def test_08(self):


        

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()