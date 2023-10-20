#include <ENCRYPTO_utils/parse_options.h>
//#include "../../abycore/aby/abyparty.h"
#include <abycore/aby/abyparty.h>
#include "../HMAC512/hmac512_circuit/hmac512_circuit.h"

// Read input parameters
int32_t read_test_options(int32_t *argcp, char ***argvp, e_role *role, uint32_t *bitlen, uint32_t *secparam, std::string *address, uint16_t *port, e_sharing *sharing, std::string *key, std::string *data, uint32_t *ser)
{

	uint32_t int_role = 0, int_port = 7766, int_sharing = 0;
	bool useffc = false;

	parsing_ctx options[] = {{(void *)&int_role, T_NUM, "r", "Role: 0/1", true, false}, {(void *)bitlen, T_NUM, "b", "Bit-length, default 32", false, false}, {(void *)secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false}, {(void *)address, T_STR, "a", "IP-address, default: localhost", false, false}, {(void *)port, T_NUM, "p", "Port, default: 7766", false, false}, {(void *)&int_sharing, T_NUM, "g", "Sharing in which the hmac512 circuit should be evaluated [0: BOOL, 1: YAO], default: YAO", false, false}, {(void *)key, T_STR, "k", "key(32bytes)", false, false}, {(void *)data, T_STR, "d", "data (32bytes)", true, false}, {(void *)ser, T_NUM, "c", "the index of child key, default: 0", false, false}};

	if (!parse_options(argcp, argvp, options, sizeof(options) / sizeof(parsing_ctx)))
	{
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}

	assert(int_role < 2);
	*role = (e_role)int_role;

	if (int_port != 0)
	{
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t)int_port;
	}

	assert(int_sharing == S_BOOL || int_sharing == S_YAO);
	assert(int_sharing != S_ARITH);
	*sharing = (e_sharing)int_sharing;

	// delete options;

	return 1;
}

int main(int argc, char **argv)
{
	e_role role;
	uint32_t bitlen = 256, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	e_mt_gen_alg mt_alg = MT_OT;

	e_sharing sharing = S_YAO;

	std::string key, data;
	uint32_t ser = 0;

	read_test_options(&argc, &argv, &role, &bitlen, &secparam, &address, &port, &sharing, &key, &data, &ser);

	// ser should be bigger than 2^31
	/*
	if (ser < 2147483648)
	{
		std::cout << "It shoudn't be hardened! The index of child key is smaller than 2^31.\n";
		return 0;
	}
	*/
	if (key.length()%2 != 0)
	{
		std::cout << "Keys should be byte aligned.\n";
		return 0;
	}
	if (data.length()%2 != 0)
	{
		std::cout << "data should be byte aligned.\n";
		return 0;
	}

	seclvl seclvl = get_sec_lvl(secparam);

	test_hmac512_circuit(role, address, port, seclvl, nthreads, mt_alg, sharing, bitlen, key, data, ser);

	return 0;
}
