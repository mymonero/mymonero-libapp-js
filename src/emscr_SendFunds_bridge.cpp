//
//  emscr_async_bridge_index.cpp
//  Copyright (c) 2014-2019, MyMonero.com
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are
//  permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//	conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//	of conditions and the following disclaimer in the documentation and/or other
//	materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific
//	prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
//  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
//  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
//
#include "emscr_SendFunds_bridge.hpp"
//
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <emscripten.h>
#include <unordered_map>
#include <memory>
//
#include "string_tools.h"
#include "wallet_errors.h"
//
#include "serial_bridge_utils.hpp"
#include "SendFundsFormSubmissionController.hpp"
//
//
using namespace std;
using namespace boost;
using namespace SendFunds;
//
using namespace serial_bridge_utils;
using namespace monero_send_routine;
using namespace monero_transfer_utils;
using namespace emscr_SendFunds_bridge;
//
// Runtime - Memory
//
SendFunds::FormSubmissionController *controller_ptr = NULL;
//
// To-JS fn decls - Status updates and routine completions
static void send_app_handler__status_update(ProcessStep code)
{
	boost::property_tree::ptree root;
	root.put("code", code); // not 64bit so sendable in JSON
	auto ret_json_string = ret_json_from_root(root);
	EM_ASM_(
		{
			const JS__req_params_string = Module.UTF8ToString($0);
			const JS__req_params = JSON.parse(JS__req_params_string);
			Module.fromCpp__SendFundsFormSubmission__status_update(JS__req_params); // Module must implement this!
		},
		ret_json_string.c_str()
	);
}
void emscr_SendFunds_bridge::send_app_handler__error_json(const string &ret_json_string)
{
	EM_ASM_(
		{
			const JS__req_params_string = Module.UTF8ToString($0);
			const JS__req_params = JSON.parse(JS__req_params_string);
			Module.fromCpp__SendFundsFormSubmission__error(JS__req_params); // Module must implement this!
		},
		ret_json_string.c_str()
	);
	THROW_WALLET_EXCEPTION_IF(controller_ptr == NULL, error::wallet_internal_error, "expected non-NULL controller_ptr");
	delete controller_ptr; // having finished
	controller_ptr = NULL;
}
void emscr_SendFunds_bridge::send_app_handler__error_msg(const string &err_msg)
{
	send_app_handler__error_json(error_ret_json_from_message(std::move(err_msg)));
}
void emscr_SendFunds_bridge::send_app_handler__error_code(
	SendFunds::PreSuccessTerminalCode code,
	optional<string> msg,
	optional<CreateTransactionErrorCode> createTx_errCode,
	// for display / information purposes on errCode=needMoreMoneyThanFound during step1:
	optional<uint64_t> spendable_balance,
	optional<uint64_t> required_balance
) {
	boost::property_tree::ptree root;
	root.put(ret_json_key__any__err_code(), code);
	if (msg) {
		root.put(ret_json_key__any__err_msg(), std::move(*msg));
	}
	if (createTx_errCode != boost::none) {
		root.put("createTx_errCode", createTx_errCode);
	}
	if (spendable_balance != boost::none) {
		root.put(ret_json_key__send__spendable_balance(), std::move(RetVals_Transforms::str_from(*spendable_balance)));
	}
	if (required_balance != boost::none) {
		root.put(ret_json_key__send__required_balance(), std::move(RetVals_Transforms::str_from(*required_balance)));
	}
	send_app_handler__error_json(ret_json_from_root(root));
}
//
void send_app_handler__success(const Success_RetVals &success_retVals)
{
	boost::property_tree::ptree root;
	root.put(ret_json_key__send__used_fee(), std::move(RetVals_Transforms::str_from(success_retVals.used_fee)));
	root.put(ret_json_key__send__total_sent(), std::move(RetVals_Transforms::str_from(success_retVals.total_sent)));
	root.put(ret_json_key__send__mixin(), success_retVals.mixin); // this is a uint32 so it can be sent in JSON
	if (success_retVals.final_payment_id) {
		root.put(ret_json_key__send__final_payment_id(), std::move(*(success_retVals.final_payment_id)));
	}
	root.put(ret_json_key__send__serialized_signed_tx(), std::move(success_retVals.signed_serialized_tx_string));
	root.put(ret_json_key__send__tx_hash(), std::move(success_retVals.tx_hash_string));
	root.put(ret_json_key__send__tx_key(), std::move(success_retVals.tx_key_string));
	root.put(ret_json_key__send__tx_pub_key(), std::move(success_retVals.tx_pub_key_string));
	root.put("target_address", std::move(success_retVals.target_address));
	root.put("final_total_wo_fee", std::move(RetVals_Transforms::str_from(success_retVals.final_total_wo_fee)));
	root.put("isXMRAddressIntegrated", std::move(RetVals_Transforms::str_from(success_retVals.isXMRAddressIntegrated)));
	if (success_retVals.final_payment_id) {
		root.put("integratedAddressPIDForDisplay", std::move(*(success_retVals.integratedAddressPIDForDisplay)));
	}
	//
	EM_ASM_(
		{
			const JS__req_params_string = Module.UTF8ToString($0);
			const JS__req_params = JSON.parse(JS__req_params_string);
			Module.fromCpp__SendFundsFormSubmission__success(JS__req_params); // Module must implement this!
		},
		ret_json_from_root(root).c_str()
	);
	THROW_WALLET_EXCEPTION_IF(controller_ptr == NULL, error::wallet_internal_error, "expected non-NULL controller_ptr");
	delete controller_ptr; // having finished
	controller_ptr = NULL;
}
//
// From-JS function decls
void emscr_SendFunds_bridge::send_funds(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// (it will already have thrown an exception)
		send_app_handler__error_msg(error_ret_json_from_message("Invalid JSON"));
		return;
	}
	Parameters parameters{
		json_root.get<bool>("fromWallet_didFailToInitialize"),
		json_root.get<bool>("fromWallet_didFailToBoot"),
		json_root.get<bool>("fromWallet_needsImport"),
		//
		json_root.get<bool>("requireAuthentication"),
		//
		json_root.get<string>("sending_amount_double_string"),
		json_root.get<bool>("is_sweeping"),
		(uint32_t)stoul(json_root.get<string>("priority")),
		//
		json_root.get<bool>("hasPickedAContact"),
		json_root.get_optional<string>("contact_payment_id"),
		json_root.get_optional<bool>("contact_hasOpenAliasAddress"),
		json_root.get_optional<string>("cached_OAResolved_address"),
		json_root.get_optional<string>("contact_address"),
		//
		nettype_from_string(json_root.get<string>("nettype_string")),
		json_root.get<string>("from_address_string"),
		json_root.get<string>("sec_viewKey_string"),
		json_root.get<string>("sec_spendKey_string"),
		json_root.get<string>("pub_spendKey_string"),
		//
		json_root.get_optional<string>("enteredAddressValue"),
		//
		json_root.get_optional<string>("resolvedAddress"),
		json_root.get<bool>("resolvedAddress_fieldIsVisible"),
		//
		json_root.get_optional<string>("manuallyEnteredPaymentID"),
		json_root.get<bool>("manuallyEnteredPaymentID_fieldIsVisible"),
		//
		json_root.get_optional<string>("resolvedPaymentID"),
		json_root.get<bool>("resolvedPaymentID_fieldIsVisible"),
		//
		[] ( // preSuccess_nonTerminal_validationMessageUpdate_fn
			ProcessStep step
		) -> void {
			send_app_handler__status_update(step);
		},
		[] ( // failure_fn
			SendFunds::PreSuccessTerminalCode code,
			optional<string> msg,
			optional<CreateTransactionErrorCode> createTx_errCode,
			optional<uint64_t> spendable_balance,
			optional<uint64_t> required_balance
		) -> void {
			send_app_handler__error_code(code, msg, createTx_errCode, spendable_balance, required_balance);
		},
		[] () -> void { // preSuccess_passedValidation_willBeginSending
			EM_ASM_(
				{
					Module.fromCpp__SendFundsFormSubmission__willBeginSending({}); // Module must implement this!
				}
			);
		},
		//
		[] () -> void { // canceled_fn
			EM_ASM_(
				{
					Module.fromCpp__SendFundsFormSubmission__canceled({}); // Module must implement this!
				}
			);
			THROW_WALLET_EXCEPTION_IF(controller_ptr == NULL, error::wallet_internal_error, "expected non-NULL controller_ptr");
			delete controller_ptr; // having finished
			controller_ptr = NULL;
		},
		[] (SendFunds::Success_RetVals retVals) -> void // success_fn
		{
			send_app_handler__success(retVals);
		}
	};
	controller_ptr = new FormSubmissionController{parameters}; // heap alloc
	if (!controller_ptr) { // exception will be thrown if oom but JIC, since null ptrs are somehow legal in WASM
		send_app_handler__error_msg("Out of memory (heap vals container)");
		return;
	}
	(*controller_ptr).set__authenticate_fn(
		[] () -> void
		{ // authenticate_fn - this is not guaranteed to be called but it will be if requireAuthentication is true
			EM_ASM_(
				{
					Module.fromCpp__SendFundsFormSubmission__authenticate(); // Module must implement this!
				}
			);
		}
	);
	(*controller_ptr).set__get_unspent_outs_fn([] (LightwalletAPI_Req_GetUnspentOuts req_params) -> void
	{ // get_unspent_outs
		boost::property_tree::ptree req_params_root;
		req_params_root.put("address", req_params.address);
		req_params_root.put("view_key", req_params.view_key);
		req_params_root.put("amount", req_params.amount);
		req_params_root.put("dust_threshold", req_params.dust_threshold);
		req_params_root.put("use_dust", req_params.use_dust);
		req_params_root.put("mixin", req_params.mixin);
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);
		EM_ASM_(
			{
				const JS__req_params_string = Module.UTF8ToString($0);
				const JS__req_params = JSON.parse(JS__req_params_string);
				Module.fromCpp__SendFundsFormSubmission__get_unspent_outs(JS__req_params); // Module must implement this!
			},
			req_params_ss.str().c_str()
		);
	});
	(*controller_ptr).set__get_random_outs_fn([] (LightwalletAPI_Req_GetRandomOuts req_params) -> void
	{ // get_random_outs
		boost::property_tree::ptree req_params_root;
		boost::property_tree::ptree amounts_ptree;
		BOOST_FOREACH(const string &amount_string, req_params.amounts)
		{
			property_tree::ptree amount_child;
			amount_child.put("", amount_string);
			amounts_ptree.push_back(std::make_pair("", amount_child));
		}
		req_params_root.add_child("amounts", amounts_ptree);
		req_params_root.put("count", req_params.count);
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);
		EM_ASM_(
			{
				const JS__req_params_string = Module.UTF8ToString($0);
				const JS__req_params = JSON.parse(JS__req_params_string);
				Module.fromCpp__SendFundsFormSubmission__get_random_outs(JS__req_params); // Module must implement this!
			},
			req_params_ss.str().c_str()
		);
	});
	(*controller_ptr).set__submit_raw_tx_fn([] (LightwalletAPI_Req_SubmitRawTx req_params) -> void
	{ // submit_raw_tx
		boost::property_tree::ptree req_params_root;
		boost::property_tree::ptree amounts_ptree;
		req_params_root.put("address", std::move(req_params.address));
		req_params_root.put("view_key", std::move(req_params.view_key));
		req_params_root.put("tx", std::move(req_params.tx));
		stringstream req_params_ss;
		boost::property_tree::write_json(req_params_ss, req_params_root, false/*pretty*/);
		auto req_params_string = req_params_ss.str();
		EM_ASM_(
			{
				const JS__req_params_string = Module.UTF8ToString($0);
				const JS__req_params = JSON.parse(JS__req_params_string);
				Module.fromCpp__SendFundsFormSubmission__submit_raw_tx(JS__req_params); // Module must implement this!
			},
			req_params_ss.str().c_str()
		);
	});
	(*controller_ptr).handle();
}
//
void emscr_SendFunds_bridge::send_cb__authentication(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// (it will already have thrown an exception)
		send_app_handler__error_msg(error_ret_json_from_message("Invalid JSON"));
		return;
	}
	auto did_pass = json_root.get<bool>("did_pass");
	if (!controller_ptr) { // an error will have been returned already - just bail.
		return;
	}
	(*controller_ptr).cb__authentication(did_pass);
}
void emscr_SendFunds_bridge::send_cb_I__got_unspent_outs(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// (it will already have thrown an exception)
		send_app_handler__error_msg(error_ret_json_from_message("Invalid JSON"));
		return;
	}
	auto optl__err_msg = json_root.get_optional<string>("err_msg");
	if (optl__err_msg != boost::none && (*optl__err_msg).size() > 0) { // if args_string actually contains a server error, call error fn with it - this must be done so that the heap alloc'd vals container can be freed
		stringstream err_msg_ss;
		err_msg_ss << "An error occurred while getting your latest balance: " << *(optl__err_msg);
		send_app_handler__error_msg(err_msg_ss.str());
		return;
	}
	if (!controller_ptr) { // an error will have been returned already - just bail.
		return;
	}
	(*controller_ptr).cb_I__got_unspent_outs(optl__err_msg, json_root.get_child("res"));
}
void emscr_SendFunds_bridge::send_cb_II__got_random_outs(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// (it will already have thrown an exception)
		send_app_handler__error_msg(error_ret_json_from_message("Invalid JSON"));
		return;
	}
	auto optl__err_msg = json_root.get_optional<string>("err_msg");
	if (optl__err_msg != boost::none && (*optl__err_msg).size() > 0) { // if args_string actually contains a server error, call error fn with it - this must be done so that the heap alloc'd vals container can be freed
		stringstream err_msg_ss;
		err_msg_ss << "An error occurred while getting decoy outputs: " << *(optl__err_msg);
		send_app_handler__error_msg(err_msg_ss.str());
		return;
	}
	if (!controller_ptr) { // an error will have been returned already - just bail.
		return;
	}
	(*controller_ptr).cb_II__got_random_outs(optl__err_msg, json_root.get_child("res"));
}
void emscr_SendFunds_bridge::send_cb_III__submitted_tx(const string &args_string)
{
	boost::property_tree::ptree json_root;
	if (!parsed_json_root(args_string, json_root)) {
		// (it will already have thrown an exception)
		send_app_handler__error_msg(error_ret_json_from_message("Invalid JSON"));
		return;
	}
	auto optl__err_msg = json_root.get_optional<string>("err_msg");
	if (optl__err_msg != boost::none && (*optl__err_msg).size() > 0) { // if args_string actually contains a server error, call error fn with it - this must be done so that the heap alloc'd vals container can be freed
		stringstream err_msg_ss;
		err_msg_ss << "An error occurred while submitting transaction: " << *(optl__err_msg);
		send_app_handler__error_msg(err_msg_ss.str());
		return;
	}
	if (!controller_ptr) { // an error will have been returned already - just bail.
		return;
	}
	(*controller_ptr).cb_III__submitted_tx(optl__err_msg);
}
