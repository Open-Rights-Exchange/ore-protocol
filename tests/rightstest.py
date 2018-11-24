# python3 ./tests/rightstest.py

import time
import setup
import eosf
import node
import unittest
from termcolor import cprint

setup.set_verbose(True)
setup.set_json(False)
setup.use_keosd(False)
#setup.set_command_line_mode(True)

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

        rights_deploy = eosf.account(account_master, name="rights.ore")
        wallet.import_key(rights_deploy)
        assert(not rights_deploy.error)

        contract_eosio_bios = eosf.Contract(
            account_master, "eosio.bios").deploy()
        assert(not contract_eosio_bios.error)

        global rights_ore
        rights_ore = eosf.Contract(rights_deploy, contractPath+"/ore.rights_registry")
        assert(not rights_ore.error)


        deployment = rights_ore.deploy()
        assert(not deployment.error)


    def setUp(self):
        pass


    def test_01(self):

        cprint("""
RIGHT REGISTRY TEST STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("upsertright", app_apim)
        """, 'magenta')
        self.assertFalse(rights_ore.push_action(
            "upsertright",
            '{"issuer":"' + 'test1'
                + '","right_name":"TestRight1","urls":[{"url":"https://url/sample","method":"get","matches_params":[],"token_life_span":100,"is_default":1}],"issuer_whitelist":["test1"]}'
            , test1, output=True).error)

        cprint("""
Assign t1 = rights_ore.table("rights.ore", "rights.ore")
        """, 'green')
        t1 = rights_ore.table("rights", "rights.ore")


        cprint("""
RIGHT UDPDATE TEST STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("upsertright", app_apim)
        """, 'magenta')
        self.assertFalse(rights_ore.push_action(
            "upsertright",
            '{"issuer":"' + 'test1'
                + '","right_name":"TestRight1","urls":[{"url":"https://url/edited1","method":"get","matches_params":[],"token_life_span":100,"is_default":1}],"issuer_whitelist":["test1", "test2"]}'
            , test1, output=True).error)

        cprint("""
Assign t1 = rights_ore.table("rights.ore", "rights.ore")
        """, 'green')
        t2 = rights_ore.table("rights", "rights.ore")



        cprint("""
RIGHT UPDATE FAIL TEST STARTED
        """, 'yellow')

        cprint("test2 account tries to update TestRight1, which is issued by test1", 'magenta')
        self.assertTrue(rights_ore.push_action(
            "upsertright",
            '{"issuer":"' + 'test2'
                + '","right_name":"TestRight1","urls":[{"url":"https://url/edited2","method":"get","matches_params":[],"token_life_span":100,"is_default":1}],"issuer_whitelist":["test1", "test2", "test3"]}'
            , test2, output=True).error)

        cprint("""
Assign t1 = rights_ore.table("rights.ore", "rights.ore")
        """, 'green')
        t3 = rights_ore.table("rights", "rights.ore")

    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        node.stop()


if __name__ == "__main__":
    unittest.main()