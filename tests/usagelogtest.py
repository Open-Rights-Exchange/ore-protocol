# python3 ./tests/mint.py

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

        usage_deploy = eosf.account(account_master, name="usagelog")
        wallet.import_key(usage_deploy)
        assert(not usage_deploy.error)

        contract_eosio_bios = eosf.Contract(
            account_master, "eosio.bios").deploy()
        assert(not contract_eosio_bios.error)

        global usagelog_ore
        usagelog_ore = eosf.Contract(usage_deploy, contractPath+"/ore.usage_log")
        assert(not usagelog_ore.error)


        deployment = usagelog_ore.deploy()
        assert(not deployment.error)


    def setUp(self):
        pass


    def test_01(self):

        cprint("""
USAGE LOG TESTS STARTED
        """, 'yellow')

        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "createlog",
            '{"instrument_id": 0, "token_hash":"tokenhash", "timestamp":12345678 }').error)
        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "createlog",
            '{"instrument_id": 1, "token_hash":"tokenhash", "timestamp":12345678 }').error)
        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "createlog",
            '{"instrument_id": 2, "token_hash":"tokenhash", "timestamp":12345678 }').error)
        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertTrue(usagelog_ore.push_action(
            "createlog",
            '{"instrument_id": 2, "token_hash":"tokenhash", "timestamp":12345678 }').error)

        cprint("""
Assign t1 = usagelog_ore.table("logs", "usagelog")
        """, 'green')
        t1 = usagelog_ore.table("logs", "usagelog")

    def test_02(self):

        cprint("""
USAGE COUNT TESTS STARTED
        """, 'yellow')

        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "updatecount",
            '{"instrument_id": 0, "right_name":"rightname", "cpu":"10.0000 CPU" }').error)

        time.sleep(1)
        
        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "updatecount",
            '{"instrument_id": 0, "right_name":"rightname", "cpu":"10.0000 CPU" }').error)

        time.sleep(1)

        cprint("""
Action usagelog_ore.push_action("LOG CREATE")
        """, 'magenta')
        self.assertFalse(usagelog_ore.push_action(
            "updatecount",
            '{"instrument_id": 0, "right_name":"rightname", "cpu":"10.0000 CPU" }').error)

        time.sleep(1)

        cprint("""
Assign t1 = usagelog_ore.table("counts", "usagelog")
        """, 'green')
        t1 = usagelog_ore.table("counts", "0")


    def tearDown(self):
        pass


    @classmethod
    def tearDownClass(cls):
        pass


if __name__ == "__main__":
    unittest.main()