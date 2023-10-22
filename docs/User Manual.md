# Introduction

This project implements the generation of hardened child keys described in [BIP32](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki#user-content-Child_key_derivation_CKD_functions). The inputs of the function are mainly chain code, parent private/public key and index, and the output is IL||IR. IL (32 bytes) is the arithmetic shared value of both parties and will be used to generate the child private/public  key. IR (32 bytes)  is the newly generated chain code. 

# How to deploy

## Requirements

- Since this project uses the ABY framework, the requirements are similar to ABY.

- The **Linux distribution** of your choice . (This project currently only supports x86 architecture and was developed and tested with Ubuntu18.04)

- **Required packages**:

  - [`g++`](https://packages.debian.org/testing/g++) (version >=8) or another compiler and standard library implementing C++17 including the filesystem library
  - [`make`](https://packages.debian.org/testing/make)
  - [`cmake`](https://packages.debian.org/testing/cmake)
  - [`libgmp-dev`](https://packages.debian.org/testing/libgmp-dev)
  - [`libssl-dev`](https://packages.debian.org/testing/libssl-dev)
  - [`libboost-all-dev`](https://packages.debian.org/testing/libboost-all-dev) (version >= 1.66)

  Install these packages with your favorite package manager, e.g, `sudo apt-get install <package-name>`.

## Build

1. First pull this project to the target folder.

   ```
   $ git clone https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key.git
   ```

2. Enter the root directory of this project, create a new `build` folder here, and enter.

   ```
   $ cd MPC_BIP32_Hardened_Child_Key-master
   $ mkdir build
   $ cd build
   ```

	3. Configure and build the project using CMake.

    ```
    $ cmake ..
    ```

    - This also initializes and updates the Git submodules of the dependencies located in `extern/`. If you plan to work without a network connection, you should to a `--recursive` clone in Step 1.

	4. Call `make` in the `build/` directory. You can find the build executables and libraries in the directories `bin/` and `lib/`, respectively.

    ```
    $ make
    ```

# Test

- Enter the `build/bin/` folder and you can see the generated executable file `hmac512_test`

  ```
  $ cd bin
  ```

- Run the executable file, open two terminals (client and server respectively), and enter the following commands respectively.

  - Server: 

    ```
    ./hmac512_test -r 0 -k 01 -c 63 -c 0
    ```

  - Client: 

    ```
    ./hmac512_test -r 1 -k 61 -c 63 -c 0
    ```

- You will see output similar to the following:

  - Server: 

    ```
    Chain code:	6300000000000000000000000000000000000000000000000000000000000000
    Parent key:	0100000000000000000000000000000000000000000000000000000000000000
    index:	0
    IL（BOOL Share）:	2e7f81e03571067b7c3fcb2fb8fae742b90f8b3103cc41055dc93d97c308abbe
    IR（Chaincode）:	2ada08bca3daabad1745d6885f7a95eda0e86acc86e945cde775ebf7fcadcb31
    IL（Arith Share）:	525598b3f8a99aaa6bb4984056740181dfd1cf5512fc58c22de9613d71d6f01a
    ```

  - Client: 

    ```
    Chain code:	6300000000000000000000000000000000000000000000000000000000000000
    Parent key:	6100000000000000000000000000000000000000000000000000000000000000
    index:	0
    IL（BOOL Share）:	5fec80e9992fefd231588a4eee6946b8d821070708b99a4de4f7d45220c7e500
    IR（Chaincode）:	2ada08bca3daabad1745d6885f7a95eda0e86acc86e945cde775ebf7fcadcb31
    IL（Arith Share）:	1f3d6855b3b54efee1b2a921001fa078815cbce0f87982868b55888871f85ea4
    ```

  - Since the generation process of IL (BOOL Share), IR (Chaincode), and IL (Arith Share) is random, the results of each run will be different.

- **Parameter Description**：

  - The above test example only involve several parameters. You can configure them according to your specific needs. The details of the parameters are as follows:

    ```
     -r ：role; 0 is for server and 1 is for client; required.
     -b ：Shared bit length; default value is 256; optional.
     -s ：System security parameter; default value is 128; optional.
     -a ：IP address; default is localhost; optional.
     -p ：Port number; default value is 7766; optional.
     -c ：Chain code (32 bytes); required.
     -k ：Parent public/private key (32 bytes); required.
     -c ：Index of child public/private key; required.
    ```

  - When the chain code and parent extended private key are input, the format is hexadecimal little-endian and needs to be aligned by bytes. For example, the string "abc" should be converted to "616263".

# Notice

- ABY code provides two gates that should only be used during development and test: the PrintValue gate and the Assert gate. These two gates cannot be used when working with private data because they will leak intermediate values. To disable these gates, set the macro `ABY_PRODUCTION` to 1 in `/extern/ABY/src/abycore/ABY_utils/ABYconstants.h:25`.