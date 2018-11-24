# python3 ./tests/mint.py

import setup
import eosf
import node
import unittest
from termcolor import cprint

setup.set_verbose(True)
setup.set_json(False)
setup.use_keosd(False)

class Test1(unittest.TestCase):

    def run(self, result=None):
        """ Stop after first error """
        if not result.failures:
            super().run(result)


    @classmethod
    def setUpClass(cls):
        global contractPath
        # set this variable to the local path for the contracts folder
        contractPath = "/Users/basar/Workspace/ore-protocol/contracts"

        testnet = node.reset()
        assert(not testnet.error)

        wallet = eosf.Wallet()
        assert(not wallet.error)

        global account_master
        account_master = eosf.AccountMaster()
        wallet.import_key(account_master)
        assert(not account_master.error)

        global test1
        test1 = eosf.account(account_master, name="test1")
        wallet.import_key(test1)
        assert(not test1.error)

        global test2
        test2 = eosf.account(account_master, name="test2")
        wallet.import_key(test2)
        assert(not test2.error)

        global test3
        test3 = eosf.account(account_master, name="test3")
        wallet.import_key(test3)
        assert(not test3.error)

        contract_deploy = eosf.account(account_master, name="token.ore")
        wallet.import_key(contract_deploy)
        assert(not contract_deploy.error)
        
        contract_eosio_bios = eosf.Contract(
            account_master, "eosio.bios").deploy()
        assert(not contract_eosio_bios.error)
 
        global token_contract
        token_contract = eosf.Contract(contract_deploy, contractPath+"/ore.standard_token")
        assert(not token_contract.error)

        deployment = token_contract.deploy()
        assert(not deployment.error)


    def setUp(self):
        pass


    def test_01(self):

        cprint("""
TOKEN CREATE TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "create",
            '{"issuer":"'
                + 'test1'
                + '", "maximum_supply":"100000000.0000 ORERERERERE"}').error)

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "create",
            '{"issuer":"'
                + 'test1'
                + '", "maximum_supply":"-100000000.0000 OREE"}').error)

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "create",
            '{"issuer":"'
                + 'test1'
                + '", "maximum_supply":"100000000.0000 ORE"}').error)

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "create",
            '{"issuer":"'
                + 'test1'
                + '", "maximum_supply":"100000000.0000 ORE"}').error)

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "issue",
            '{"to":"test1", "quantity":"10000000.0000 ORE", "memo": "ORE token issued"}',
                test1).error)

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "issue",
            '{"to":"test1", "quantity":"100000000000000.0000 ORE", "memo": "ORE token issued"}',
                test1).error)

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "issue",
            '{"to":"test2", "quantity":"1000000.0000 ORE", "memo": "ORE token issued"}',
                test2).error)

    def test_02(self):

        cprint("""
TOKEN TRANSFER TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "transfer",
            '{"from":"test1", "to":"test2", "quantity":"100.0000 ORE", "memo":"transfer1"}', test1).error)

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "transfer",
            '{"from":"test2", "to":"test3", "quantity":"50.0000 ORE", "memo":"transfer1"}', test2).error)

        cprint("""
Action contract.push_action("ORE transfer")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "transfer",
            '{"from":"test2", "to":"test3", "quantity":"60.0000 ORE", "memo":"transfer1"}', test2).error)

    def test_03(self):

        cprint("""
TOKEN APPROVAL TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "approve",
            '{"from":"test1", "to":"test3", "quantity":"100.0000 ORE", "memo":"transfer1"}', test1).error)

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "approve",
            '{"from":"test2", "to":"test3", "quantity":"100.0000 ORE", "memo":"transfer1"}', test1).error)

    def test_04(self):

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "transferfrom",
            '{"sender":"test3", "from":"test1", "to":"test2", "quantity":"100.0000 ORE"}', test3).error)

        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertTrue(token_contract.push_action(
            "transferfrom",
            '{"sender":"test3", "from":"test1", "to":"test2", "quantity":"100.0000 ORE"}', test3).error)

    def test_05(self):

        cprint("""
TOKEN TABLE TESTS STARTED
        """, 'yellow')

        cprint("""
Assign t1 = token_contract.table("rights.ore", "rights.ore")
        """, 'green')
        t1 = token_contract.table("accounts", "test1")

        cprint("""
Assign t1 = token_contract.table("rights.ore", "rights.ore")
        """, 'green')
        t2 = token_contract.table("accounts", "test2")

        cprint("""
Assign t1 = token_contract.table("rights.ore", "rights.ore")
        """, 'green')
        t2 = token_contract.table("accounts", "test3")

        cprint("""
Assign t1 = token_contract.table("rights.ore", "rights.ore")
        """, 'green')
        t3 = token_contract.table("allowances", "test1")


    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        pass


if __name__ == "__main__":
    unittest.main()