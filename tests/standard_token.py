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
        create_account("nonapp", master)


    def setUp(self):
        pass


    def test_01(self):
        COMMENT('''
        Create, build and deploy the contracts:
        ''')
        create_account("token", master)
        token_contract = Contract(token, "/root/ore-protocol/contracts/ore.standard_token")
        token_contract.build()
        token_contract.deploy()

    def test_02(self):
        COMMENT('''
        Create tokens:
        ''')

        #Valid transaction
        token.push_action(
                "create",
                {
                    "issuer": token,
                    "maximum_supply": "100000000.0000 ORE"
                },
                permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Invalid symbol name
            token.push_action(
                    "create",
                    {
                        "issuer": token,
                        "maximum_supply": "100000000.0000 OREEEEEE"
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Invalid supply
            token.push_action(
                    "create",
                    {
                        "issuer": token,
                        "maximum_supply": "100000.000000000000000000000000000000 ORA"
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Max supply must be positive
            token.push_action(
                    "create",
                    {
                        "issuer": token,
                        "maximum_supply": "-100000000.0000 ORA"
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Token symbol already exists
            token.push_action(
                    "create",
                    {
                        "issuer": token,
                        "maximum_supply": "100000000.0000 ORE"
                    },
                    permission=(token, Permission.ACTIVE))


    def test_03(self):
        COMMENT('''
        Issue tokens:
        ''')
        #Valid transaction
        token.push_action(
                "issue",
                {
                    "to": app,
                    "quantity": "10000000.0000 ORE",
                    "memo": ""
                },
                permission=(token, Permission.ACTIVE))
                
        token.push_action(
                "issue",
                {
                    "to": token,
                    "quantity": "100.0000 ORE",
                    "memo": ""
                },
                permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Invalid symbol name
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "100.0000 OREEEEEE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Token symbol does not exists
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "100.0000 ORU",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Memo has more than 256 bytes
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "100.0000 ORE",
                        "memo": "LONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONG"
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Quantity must be positive
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "-100.0000 ORE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Symbol precision mismatch
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "100.000 ORE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Quantity exceeds available supply
            token.push_action(
                    "issue",
                    {
                        "to": app,
                        "quantity": "100000000.0000 ORE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

    def test_04(self):
        COMMENT('''
        Approve tokens:
        ''')
        #Valid transaction
        token.push_action(
                "approve",
                {
                    "from": app,
                    "to": nonapp,
                    "quantity": "10.0000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Amount being approved is more than the balance of approver account
            token.push_action(
                    "approve",
                    {
                        "from": app,
                        "to": nonapp,
                        "quantity": "10000001.0000 ORE",
                        "memo": ""
                    },
                    permission=(app, Permission.ACTIVE))

    def test_05(self):
        COMMENT('''
        Retire tokens:
        ''')
        #Valid transaction
        token.push_action(
                "retire",
                {
                    "quantity": "100.0000 ORE",
                    "memo": ""
                },
                permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Invalid symbol name
            token.push_action(
                    "retire",
                    {
                        "quantity": "100.0000 OREEEEEE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Token symbol does not exists
            token.push_action(
                    "retire",
                    {
                        "quantity": "100.0000 ORU",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Memo has more than 256 bytes
            token.push_action(
                    "retire",
                    {
                        "quantity": "100.0000 ORE",
                        "memo": "LONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONG"
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Quantity must be positive
            token.push_action(
                    "retire",
                    {
                        "quantity": "-100.0000 ORE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))

        with self.assertRaises(Error): #Symbol precision mismatch
            token.push_action(
                    "retire",
                    {
                        "quantity": "100.000 ORE",
                        "memo": ""
                    },
                    permission=(token, Permission.ACTIVE))


    def test_06(self):
        COMMENT('''
        Transfer tokens:
        ''')
        #Valid Transaction
        token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": nonapp,
                    "quantity": "10.0000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Cannot transfer to self
            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": app,
                    "quantity": "100.0000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #To account does not exist
            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": "nonexist",
                    "quantity": "100.0000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Memo has more than 256 bytes
            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": nonapp,
                    "quantity": "100.0000 ORE",
                    "memo": "LONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONGLONG"
                },
                permission=(app, Permission.ACTIVE))
        
        with self.assertRaises(Error): #Invalid quantity
            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": nonapp,
                    "quantity": "A ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Quantity must be positive

            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": nonapp,
                    "quantity": "-100.0000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

        with self.assertRaises(Error): #Symbol precision mismatch
            token.push_action(
                "transfer",
                {
                    "from":app,
                    "to": nonapp,
                    "quantity": "100.000 ORE",
                    "memo": ""
                },
                permission=(app, Permission.ACTIVE))

    
    def test_07(self):
        COMMENT('''
        Transfer From tokens:
        ''')
        token.push_action(
                "transferfrom",
                {
                    "sender": nonapp,
                    "from":app,
                    "to": token,
                    "quantity": "5.0000 ORE",
                    "memo": ""
                },
                permission=(nonapp, Permission.ACTIVE))

        with self.assertRaises(Error): #The amount being transferred is more than the approved account
            token.push_action(
                    "transferfrom",
                    {
                        "sender": nonapp,
                        "from":app,
                        "to": token,
                        "quantity": "6.0000 ORE",
                        "memo": ""
                    },
                    permission=(nonapp, Permission.ACTIVE))

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        stop()


if __name__ == "__main__":
    unittest.main()
