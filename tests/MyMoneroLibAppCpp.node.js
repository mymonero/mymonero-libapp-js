// Copyright (c) 2014-2018, MyMonero.com
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//	conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//	of conditions and the following disclaimer in the documentation and/or other
//	materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific
//	prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"use strict";
//
function tests(Module)
{
	console.log("Module is of type:", typeof Module)
	{
		console.time("decode_address")
		const args =
		{
			"address": "43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg",
			"nettype_string": "MAINNET"
		}
		const ret_string = Module.decode_address(JSON.stringify(args))
		console.timeEnd("decode_address")
		console.log("decode_address ret", ret_string)
	}
	{
		console.time("is_subaddress")
		const args_str = '{"nettype_string":"MAINNET","address":"4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaL41VrjstLM5WevLZx"}'
		const ret_string = Module.is_subaddress(args_str)
		console.timeEnd("is_subaddress")
		console.log("is_subaddress ret", ret_string)
	}
	{
		console.time("is_integrated_address")
		const args_str = '{"nettype_string":"MAINNET","address":"4L6Gcy9TAHqPVPMnqa5cPtJK25tr7maE7LrJe67vzumiCtWwjDBvYnHZr18wFexJpih71Mxsjv8b7EpQftpB9NjPaL41VrjstLM5WevLZx"}'
		const ret_string = Module.is_integrated_address(args_str)
		console.timeEnd("is_integrated_address")
		console.log("is_integrated_address ret", ret_string)
	}
	{
		console.time("new_integrated_address")
		const args_str = '{"nettype_string":"MAINNET","address":"43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg","short_pid":"b79f8efc81f58f67"}'
		const ret_string = Module.new_integrated_address(args_str)
		console.timeEnd("new_integrated_address")
		console.log("new_integrated_address ret", ret_string)
	}
	{
		console.time("new_payment_id")
		const args_str = '{}'
		const ret_string = Module.new_payment_id(args_str)
		console.timeEnd("new_payment_id")
		console.log("new_payment_id ret", ret_string)
	}
	{
		console.time("newly_created_wallet")
		const args_str = '{"nettype_string":"MAINNET","locale_language_code":"en-US"}'
		const ret_string = Module.newly_created_wallet(args_str)
		console.timeEnd("newly_created_wallet")
		console.log("newly_created_wallet ret", ret_string)
	}
	{
		console.time("are_equal_mnemonics")
		const args_str = '{"a":"foxe selfish hum nexus juven dodeg pepp ember biscuti elap jazz vibrate biscui","b":"fox sel hum nex juv dod pep emb bis ela jaz vib bis"}'
		const ret_string = Module.are_equal_mnemonics(args_str)
		console.timeEnd("are_equal_mnemonics")
		console.log("are_equal_mnemonics ret", ret_string)
	}
	{
		console.time("mnemonic_from_seed")
		const args_str = '{"seed_string":"9c973aa296b79bbf452781dd3d32ad7f","wordset_name":"English"}'
		const ret_string = Module.mnemonic_from_seed(args_str)
		console.timeEnd("mnemonic_from_seed")
		console.log("mnemonic_from_seed ret", ret_string)
	}
	{
		console.time("seed_and_keys_from_mnemonic")
		const args_str = '{"mnemonic_string":"foxe selfish hum nexus juven dodeg pepp ember biscuti elap jazz vibrate biscui","nettype_string":"MAINNET"}'
		const ret_string = Module.seed_and_keys_from_mnemonic(args_str)
		console.timeEnd("seed_and_keys_from_mnemonic")
		console.log("seed_and_keys_from_mnemonic ret", ret_string)
	}
	{
		console.time("validate_components_for_login w seed")
		const args_str = '{"address_string":"43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg","sec_viewKey_string":"7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104","sec_spendKey_string":"4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803","seed_string":"9c973aa296b79bbf452781dd3d32ad7f","nettype_string":"MAINNET"}'
		const ret_string = Module.validate_components_for_login(args_str)
		console.timeEnd("validate_components_for_login w seed")
		console.log("validate_components_for_login w seed ret", ret_string)
	}
	{
		console.time("validate_components_for_login w both keys")
		const args_str = '{"address_string":"43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg","sec_viewKey_string":"7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104","sec_spendKey_string":"4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803","nettype_string":"MAINNET"}'
		const ret_string = Module.validate_components_for_login(args_str)
		console.timeEnd("validate_components_for_login w both keys")
		console.log("validate_components_for_login w both keys ret", ret_string)
	}
	{
		console.time("validate_components_for_login view only")
		const args_str = '{"address_string":"43zxvpcj5Xv9SEkNXbMCG7LPQStHMpFCQCmkmR4u5nzjWwq5Xkv5VmGgYEsHXg4ja2FGRD5wMWbBVMijDTqmmVqm93wHGkg","sec_viewKey_string":"7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104","nettype_string":"MAINNET"}'
		const ret_string = Module.validate_components_for_login(args_str)
		console.timeEnd("validate_components_for_login view only")
		console.log("validate_components_for_login view only ret", ret_string)
	}
	{
		console.time("address_and_keys_from_seed")
		const args_str = '{"seed_string":"9c973aa296b79bbf452781dd3d32ad7f","nettype_string":"MAINNET"}'
		const ret_string = Module.address_and_keys_from_seed(args_str)
		console.timeEnd("address_and_keys_from_seed")
		console.log("address_and_keys_from_seed ret", ret_string)
	}
	{
		console.time("estimated_tx_network_fee")
		const args_str = '{"fee_per_b":"24658","priority":"1"}'
		const ret_string = Module.estimated_tx_network_fee(args_str)
		console.timeEnd("estimated_tx_network_fee")
		console.log("estimated_tx_network_fee ret", ret_string)
	}
	{
		console.time("generate_key_image")
		const args_str = '{"sec_viewKey_string":"7bea1907940afdd480eff7c4bcadb478a0fbb626df9e3ed74ae801e18f53e104","sec_spendKey_string":"4e6d43cd03812b803c6f3206689f5fcc910005fc7e91d50d79b0776dbefcd803","pub_spendKey_string":"3eb884d3440d71326e27cc07a861b873e72abd339feb654660c36a008a0028b3","tx_pub_key":"fc7f85bf64c6e4f6aa612dbc8ddb1bb77a9283656e9c2b9e777c9519798622b2","out_index":"0"}'
		const ret_string = Module.generate_key_image(args_str)
		console.timeEnd("generate_key_image")
		console.log("generate_key_image ret", ret_string)
	}
}
console.time("Load module")
require('../libapp_js/MyMoneroLibAppBridge')({asmjs: false}).then(function(instance) // this can be switched to manually test asmjs vs wasm - can be exposed to option
{	
	console.timeEnd("Load module")
	console.log("Loaded instance")
	tests(instance.Module)
}).catch(function(e)
{
	console.error("Exception while loading module:", e)
})