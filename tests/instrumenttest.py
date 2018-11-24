# python3 ./tests/instrumenttest.py

import time
import setup
import sys
import json
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

        global app_apim
        app_apim = eosf.account(account_master, name="app.apim")
        wallet.import_key(app_apim)
        assert(not app_apim.error)

        token_deploy = eosf.account(account_master, name="ore.token")
        wallet.import_key(token_deploy)
        assert(not token_deploy.error)

        instr_deploy = eosf.account(account_master, name="instr.ore")
        wallet.import_key(instr_deploy)
        assert(not instr_deploy.error)

        rights_deploy = eosf.account(account_master, name="rights.ore")
        wallet.import_key(rights_deploy)
        assert(not rights_deploy.error)


        contract_eosio_bios = eosf.Contract(
            account_master, "eosio.bios").deploy()
        assert(not contract_eosio_bios.error)
 
        global token_contract
        token_contract = eosf.Contract(token_deploy, contractPath+"/ore.standard_token")
        assert(not token_contract.error)

        global instr_ore
        instr_ore = eosf.Contract(instr_deploy, contractPath+"/ore.instrument")
        assert(not instr_ore.error)

        global rights_ore
        rights_ore = eosf.Contract(rights_deploy, contractPath+"/ore.rights_registry")
        assert(not rights_ore.error)


        deployment = token_contract.deploy()
        assert(not deployment.error)

        deployment = instr_ore.deploy()
        assert(not deployment.error)

        deployment = rights_ore.deploy()
        assert(not deployment.error)


    def setUp(self):
        pass


    def test_01(self):

        cprint("""
OREINST TOKEN TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("OREINST create")
        """, 'magenta')
        self.assertFalse(instr_ore.push_action(
            "create",
            '{"issuer":"'
                + str(account_master)
                + '", "maximum_supply":"100000000.0000 OREINST"}').error)

        cprint("""
INSTRUMENT CREATE FAIL TEST STARTED
        """, 'yellow')

        cprint("instrument contract tries to create a symbol other than OREINST and fails", 'magenta')
        self.assertTrue(instr_ore.push_action(
            "create",
            '{"issuer":"'
                + str(account_master)
                + '", "maximum_supply":"100000000.0000 TEST"}').error)

        cprint("""
Action contract.push_action("OREINST issue")
        """, 'magenta')
        self.assertFalse(instr_ore.push_action(
            "issue",
            '{"to":"' + 'instr.ore'
                + '", "quantity":"10000000.0000 OREINST", "memo": "OREINST token issued"}',
                account_master).error)
        


    def test_02(self):

        cprint("""
RIGHT SET ACTION TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("upsertright", app_apim)
        """, 'magenta')
        self.assertFalse(rights_ore.push_action(
            "upsertright",
            '{"issuer":"' + 'app.apim'
                + '","right_name":"cloud.hadron.contest-2018-07","urls":[{"url":"https://contest-hadron-dot-partner-aikon.appspot.com/contest-1","method":"get","matches_params":[],"token_life_span":100,"is_default":1}],"issuer_whitelist":["app.apim"]}'
            , app_apim).error)

    def test_03(self):

        cprint("""
INSTRUMENT MINT TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("mint", app_apim)
        """, 'magenta')
        self.assertFalse(instr_ore.push_action(
            "mint",
            '{"minter":"' + 'app.apim'
                + '","owner":"' + 'app.apim'
                    +'","instrument":'+ '{"issuer":"app.apim", "instrument_class":"class", "description":"description", "instrument_template":"template", "security_type":"security",' 
                                            + '"rights":[], "parent_instrument_id":"0", "data":[],'
                                                + '"start_time":"0", "end_time":"0"}'
                        + ',"instrumentId":' + '0'
                            + '}'
            , app_apim).error)
        cprint("""
Assign t1 = instr_ore.table("tokens", "tokens")
        """, 'green')
        t1 = instr_ore.table("tokens", "instr.ore")

        cprint("""
Get the OREINST balance as 1.0000 ORE from the accounts table for app.apim)
        """,'green')
        t2 = instr_ore.table("accounts","app.apim")

    def test_04(self):
        
        cprint("""
INSTRUMENT TRANSFER TESTS STARTED
        """, 'yellow')

        self.assertFalse(instr_ore.push_action(
            "transfer",
            '{"sender":"app.apim", "to":"test1", "token_id":0}', app_apim).error)
        t3 = instr_ore.table("accounts","app.apim")
        t4 = instr_ore.table("accounts","test1")
    def test_05(self):

        cprint("""
INSTRUMENT APPROVE TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("approve", app_apim)
        """, 'magenta')
        self.assertFalse(instr_ore.push_action(
            "approve",
            '{"from":"' + 'test1'
                + '","to":"' + 'test2'
                    +'","token_id":'+ '0'
                        + '}'
            , test1).error)

    def test_06(self):
        cprint("""
INSTRUMENT TRANSFERFROM TESTS STARTED
        """, 'yellow')
    def test_07(self):
        cprint("""
INSTRUMENT BURN TESTS STARTED
        """, 'yellow')

    def test_07(self):
        cprint("""
INSTRUMENT BURNFROM TESTS STARTED
        """, 'yellow')
    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
       node.stop()


if __name__ == "__main__":
    unittest.main()