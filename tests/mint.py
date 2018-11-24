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
        contractPath = ""
        

        testnet = node.reset()
        assert(not testnet.error)

        wallet = eosf.Wallet()
        assert(not wallet.error)
        
        global account_master
        account_master = eosf.AccountMaster()
        wallet.import_key(account_master)
        assert(not account_master.error)

        global apim
        apim = eosf.account(account_master, name="apim")
        wallet.import_key(apim)
        assert(not apim.error)

        global test1
        test1 = eosf.account(apim, name="test1.apim")
        wallet.import_key(test1)
        assert(not test1.error)

        global test2
        test2 = eosf.account(apim, name="test2.apim")
        wallet.import_key(test2)
        assert(not test2.error)

        global manager_apim
        manager_apim = eosf.account(apim, name="manager.apim")
        wallet.import_key(manager_apim)
        assert(not manager_apim.error)

        global app_apim
        app_apim = eosf.account(apim, name="app.apim")
        wallet.import_key(app_apim)
        assert(not app_apim.error)

        global aikon_apim
        aikon_apim = eosf.account(apim, name="aikon.apim")
        wallet.import_key(aikon_apim)
        assert(not aikon_apim.error)

        global ore
        ore = eosf.account(account_master, name="ore")
        wallet.import_key(ore)
        assert(not ore.error)

        global token_ore
        token_ore = eosf.account(ore, name="token.ore")
        wallet.import_key(token_ore)
        assert(not token_ore.error)

        global instr_ore
        instr_ore = eosf.account(ore, name="instr.ore")
        wallet.import_key(instr_ore)
        assert(not instr_ore.error)

        global rights_ore
        rights_ore = eosf.account(ore, name="rights.ore")
        wallet.import_key(rights_ore)
        assert(not rights_ore.error)

        global usage_log_ore
        usage_log_ore = eosf.account(ore, name="usagelog.ore")
        wallet.import_key(usage_log_ore)
        assert(not usage_log_ore.error)

        contract_eosio_bios = eosf.Contract(
            account_master, "eosio.bios").deploy()
        assert(not contract_eosio_bios.error)
 
        global token_contract
        token_contract = eosf.Contract(token_ore, contractPath+"/ore.standard_token")
        assert(not token_contract.error)

        global instr_contract
        instr_contract = eosf.Contract(instr_ore, contractPath+"/ore.instrument")
        assert(not instr_contract.error)

        global rights_contract
        rights_contract = eosf.Contract(rights_ore, contractPath+"/ore.rights_registry")
        assert(not rights_contract.error)

        global usagelog_contract
        usagelog_contract = eosf.Contract(usage_log_ore, contractPath+"/ore.usage_log")
        assert(not usagelog_contract.error)

        global managerapim_contract
        managerapim_contract = eosf.Contract(manager_apim, contractPath+"/apim.manager")
        assert(not managerapim_contract.error)

        deployment = token_contract.deploy()
        assert(not deployment.error)

        deployment = instr_contract.deploy()
        assert(not deployment.error)

        deployment = rights_contract.deploy()
        assert(not deployment.error)

        deployment = usagelog_contract.deploy()
        assert(not deployment.error)

        deployment = managerapim_contract.deploy()
        assert(not deployment.error)


    def setUp(self):
        pass


    def test_01(self):

        cprint("""
TOKEN CONTRACT TESTS STARTED
        """, 'yellow')
        
        cprint("""
Action contract.push_action("CPU create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "create",
            '{"issuer":"'
                + str(account_master)
                + '", "maximum_supply":"100000000.0000 CPU"}').error)

        cprint("""
Action contract.push_action("CPU issue")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "issue",
            '{"to":"' + str(app_apim)
                + '", "quantity":"10000000.0000 CPU", "memo": "CPU token issued"}',
                account_master).error)
 
        cprint("""
Action contract.push_action("ORE create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "create",
            '{"issuer":"'
                + str(account_master)
                + '", "maximum_supply":"100000000.0000 ORE"}').error)

        cprint("""
Action contract.push_action("ORE issue")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "issue",
            '{"to":"' + str(app_apim)
                + '", "quantity":"10000000.0000 ORE", "memo": "ORE token issued"}',
                account_master).error)

        cprint("""
Action contract.push_action("OREINST create")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "create",
            '{"issuer":"'
                + str(account_master)
                + '", "maximum_supply":"100000000.0000 OREINST"}').error)

        cprint("""
Action contract.push_action("OREINST issue")
        """, 'magenta')
        self.assertFalse(token_contract.push_action(
            "issue",
            '{"to":"' + str(app_apim)
                + '", "quantity":"10000000.0000 OREINST", "memo": "OREINST token issued"}',
                account_master, output=True).error)

    def test_02(self):

        cprint("""
APP APIM ACTIONS TESTS STARTED
        """, 'yellow')

        cprint("""
Action contract.push_action("upsertright", app_apim)
        """, 'magenta')
        self.assertFalse(rights_contract.push_action(
            "upsertright",
            '{"issuer":"' + str(app_apim)
                + '","right_name":"cloud.hadron.contest-2018-07","urls":[{"url":"https://contest-hadron-dot-partner-aikon.appspot.com/contest-1","method":"get","matches_params":[],"token_life_span":100,"is_default":1}],"issuer_whitelist":["app.apim"]}'
            , app_apim, output=True).error)



        cprint("""
Assign t1 = rights_ore.table("rights.ore", "rights.ore")
        """, 'green')
        t2 = rights_contract.table("rights", "rights.ore")



    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        #node.stop()
        pass


if __name__ == "__main__":
    unittest.main()