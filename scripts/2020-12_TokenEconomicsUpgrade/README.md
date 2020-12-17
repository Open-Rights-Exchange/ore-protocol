## Token Economics V2 Upgrade ##

step1.setup-v1.sh run through each chain setup scripts in ./chain-setup dir to initialize a local test chain with v1 ORE Chain setup.

step2.upgrade-v2.sh runs the exact steps that will be applied to ORE Main chain with Token Economics update.

step3.migrate-stake.sh iterates through each account that has created before the v2 upgrade and sets up the staker for them specified in the "changetier" action.