/** FIO permissions contract
 *  Description:
 *
 *
 *        We will introduce a notion of a permission, a permission is a definition of information that provides some control
 *       and/or access to objects in state which are owned by the account that creates the permission.
 *       Permissions definitions will be extensible within the FIO protocol. new permissions can be added into
 *       the protocols contracts using the permmissions tables in this contract.
 *
 *
 *       The following process will be used for the definition and integration of new permissions in the FIO protocols:
 *               Step 1:  define the permission desired.
 *               Step 2: modify the fio.contracts affected by the new access to enforce and integrate the new permission.
 *               Step 3: rollout the new permission into testnet and main net using the following actions
 *                     3.1.   rollout the new version of the contracts supporting the new permission.
 *                     3.2   FIO user accounts begin using the permission as indicated in the spec.
 *
 *      The following vernacular is used throughout our design:
 *      Permission –  the name of the permission,
 *      Permission info -- the object type that is to be controlled, the name of the object to be controlled,
 *      the owning account, and also including all parameterized data used by the permission according to the
 *      business logic required (such as access levels, or other abstractions that can be set when an
 *      account grants the permission).
 *      Permission Auxilliary Info – the json definition of all of the parameterized data used by a given permission that is unique for a permission.
 *      for FIP-40 no additional data is necessary. this field provides extensibility such that we can introduce new
 *      or novel parameters and dials used by a new permission if this is necessary.
 *      Grantor – the granting/owning account of the object that relates to the permission.
 *      Object – the object that is being access controlled by a permission (for FIP-40 this is the domain).
 *      Grantee – the non grantor account that is given a permission.
 *      Access -- an account has access to a permission when a permission is granted to an account.
 *
 *
 *
 *
 *
 *
 *  @author  Ed Rotthoff
 *  @file fio.perms.cpp
 *  @license FIO Foundation ( https://github.com/fioprotocol/fio/blob/master/LICENSE )
 */

#include "fio.perms.hpp"
#include <fio.address/fio.address.hpp>
#include <fio.fee/fio.fee.hpp>
#include <fio.common/fio.common.hpp>
#include <fio.common/fiotime.hpp>
#include <fio.token/include/fio.token/fio.token.hpp>
#include <eosiolib/asset.hpp>

namespace fioio {

    class [[eosio::contract("FioPermissions")]]  FioPermissions : public eosio::contract {

    private:

        domains_table domains;
        fionames_table fionames;
        fiofee_table fiofees;
        eosio_names_table accountmap;
        config appConfig;

        permissions_table permissions;



    public:
        using contract::contract;

        FioPermissions(name s, name code, datastream<const char *> ds) : contract(s, code, ds),
                                                                         permissions(_self,_self.value),
                                                                        domains(_self, _self.value),
                                                                        fionames(_self, _self.value),
                                                                        fiofees(FeeContract, FeeContract.value),
                                                                        accountmap(_self, _self.value){

            configs_singleton configsSingleton(FeeContract, FeeContract.value);
            appConfig = configsSingleton.get_or_default(config());
        }


        [[eosio::action]]
        void
        addperm(const name &grantee_account,
                const string &permission_name,
                const string &permission_info,
                const string &object_name,
                const int64_t &max_fee,
                const string &tpid,
                const name &actor
                ) {


            print("addperm --      called. \n");


            // error if permission name is not the expected name register_address_on_domain.
            fio_400_assert(permission_name.compare("register_address_on_domain") == 0, "permission_name", permission_name,
                           "Permission name is invalid", ErrorInvalidPermissionName);
            // error if permission info is not empty.
            fio_400_assert(permission_info.size()  == 0, "permission_info", permission_info,
                           "Permission info is invalid", ErrorInvalidPermissionInfo);
            // error if object name is not * or is not in the domains table
            fio_400_assert(object_name.size()  > 0, "object_name", object_name,
                           "Object name is invalid", ErrorInvalidObjectName);
            // TODO error if the object name is not a domain owned by the actor.
            // error if the grantee account does not exist.
            fio_400_assert(is_account(grantee_account), "grantee_account", grantee_account.to_string(),
                           "grantee account is invalid", ErrorInvalidGranteeAccount);
            // TODO error if the grantee account already has this permission.


            //look for the permission in the permissions table. if it exists, verify all info, then note the id.
            //if it does not exist then insert it, and note the id.
            //look for an existing entry in the access table for this id, and grantee account.
            //if the entry exists then error permission exists for this account.
            //if it doesnt exist then add the record to the access table.




            /*
            const string controlv = "stuff" + object_name + permission_name;

            const uint64_t id = permissions.available_primary_key();
            //just blindly emplace for prototyping.
            permissions.emplace(get_self(), [&](struct permission_info &p) {
                p.id = id;
                p.object_type = "stuff";
                p.object_type_hash = string_to_uint128_hash("stuff");
                p.object_name = object_name;
                p.object_name_hash = string_to_uint128_hash(object_name);
                p.permission_name = permission_name;
                p.permission_name_hash = string_to_uint128_hash(permission_name);
                p.permission_control_hash = string_to_uint128_hash(controlv);
                p.owner_account = actor.value;
                p.auxilliary_info = "";
            });
            */

            const string response_string = "{\"status\": \"OK\", \"fee_collected\" : 0}";

            send_response(response_string.c_str());

        }

        [[eosio::action]]
        void
        remperm(const name &grantee_account,
                const string &permission_name,
                const string &object_name,
                const int64_t &max_fee,
                const string &tpid,
                const name &actor
        ) {


            print("remperm --      called. \n");


            uint128_t permnamehash = string_to_uint128_hash(permission_name);
            auto permsbypermname = permissions.get_index<"bypermname"_n>();
            auto bynameiter = permsbypermname.find(permnamehash);
            int c = 0;
            while (bynameiter != permsbypermname.end()) {
                bynameiter++;
                if (permission_name.compare(bynameiter->permission_name)==0) {
                    c++;
                }
            }

            print("remperm --      there are this many rows with name of. "+permission_name+ " count : "+to_string(c)+" \n");

            const string response_string = "{\"status\": \"OK\", \"fee_collected\" : 0}";

            send_response(response_string.c_str());

        }


    };

    EOSIO_DISPATCH(FioPermissions, (addperm)(remperm))
}
