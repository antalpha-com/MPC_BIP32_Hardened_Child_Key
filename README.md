# MPC_BIP32_Hardened_Child_Key
- [中文版](#中文版)
  - [介绍](#介绍)
  - [教程](#教程)
  - [技术原理](#技术原理)
  - [社区讨论](#社区讨论)
  - [参考文献](#参考文献)
- [English Version](#english-version)
  - [Introduction](#introduction)
  - [Tutorial](#tutorial)
  - [Technical principles](#technical-principles)
  - [Community discussion](#community-discussion)
  - [References](#references)
  - [License](#License)

# 中文版

## 介绍

MPC，即“多方计算”，它是一种密码协议，使多个参与方能够共同进行计算一个特定的函数，同时保持这些参与方的输入保密。MPC是一种非常强大的技术，广泛应用于隐私保护数据分析、安全多方机器学习和隐私保护金融计算等领域。

比特币BIP-32规范详细定义了HD算法原理和各种推导规则。BIP39、BIP43、BIP44都是兼容BIP32的协议，后来还被比特币以外的区块链所借鉴，形成了区块链行业的共识。Child key derivation (CKD) 指的是给出一个扩展密钥和索引，方法期望能够计算出对应的子扩展密钥。

本项目实现了两方BIP32硬化子密钥派生，输入链码（chain code）、扩展密钥分片以及子扩展密钥索引，可以生成新的链码以及用于计算子扩展密钥的IL。密钥派生的具体过程详见[bip-0032.mediawiki](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki#user-content-Child_key_derivation_CKD_functions)。

## 教程

通过下面的步骤可以对本项目进行部署、运行和测试。

1. 环境准备

2. 项目部署

3. 测试

5. 接口使用

6. 注意事项

以上步骤的具体操作请查阅[用户手册](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/blob/master/docs/用户手册.md)。

## 技术原理

本项目利用[ABY框架](https://github.com/encryptogroup/ABY)中的姚氏电路实现了两方的HMAC512运算，并将结果转换为布尔共享。使用OT扩展、大数运算等技术实现了大数的布尔共享与算数共享（B2A）的转换。

本项目使用了姚氏共享的优化方案，采用点置换混淆（Point and Permute）、Free XOR、半门技术（Half Gates）、固定密钥AES（构建混淆电路）、OT扩展、SIMD等方法，提高了计算的效率、减少了通信的开销。

## 社区讨论

这里是一个开放、平等、包容的MPC企业社区，在这里你可以与其他用户和贡献者联系，可以获得帮助并参与其中。对于任何错误或请求，您可以在 GitHub 上[报告](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/issues)并获得答案。

## 参考文献

[1] Demmler, Daniel, Thomas Schneider, and Michael Zohner. "ABY-A framework for efficient mixed-protocol secure two-party computation." *NDSS*. 2015.

[2] Bellare, Mihir, et al. "Efficient garbling from a fixed-key blockcipher." *2013 IEEE Symposium on Security and Privacy*. IEEE, 2013.

[3] Zahur, Samee, Mike Rosulek, and David Evans. "Two halves make a whole: Reducing data transfer in garbled circuits using half gates." *Advances in Cryptology-EUROCRYPT 2015: 34th Annual International Conference on the Theory and Applications of Cryptographic Techniques, Sofia, Bulgaria, April 26-30, 2015, Proceedings, Part II 34*. Springer Berlin Heidelberg, 2015.

[4] Kolesnikov, Vladimir, and Thomas Schneider. "Improved garbled circuit: Free XOR gates and applications." *Automata, Languages and Programming: 35th International Colloquium, ICALP 2008, Reykjavik, Iceland, July 7-11, 2008, Proceedings, Part II 35*. Springer Berlin Heidelberg, 2008.

[5] Beaver, Donald, Silvio Micali, and Phillip Rogaway. "The round complexity of secure protocols." *Proceedings of the twenty-second annual ACM symposium on Theory of computing*. 1990.

# English Version

## Introduction

Multi-Party Computation（MPC） is a cryptographic protocol that enables multiple parties to jointly compute a specific function while keeping the inputs of those parties confidential. MPC is a very powerful technology that is widely used in fields such as privacy-preserving data analysis, secure multi-party machine learning, and privacy-preserving financial computing. 

The Bitcoin Improvement Proposals 32 (BIP32) defines the HD algorithm principle and various key derivation rules. BIP39, BIP43, and BIP44 are all protocols compatible with BIP32, and were later adopted by other blockchains, forming a consensus in the blockchain industry. 

In the Child key derivation (CKD) functions of BIP32, given a parent extended key and an index i, it is possible to compute  the corresponding child extended key. The algorithm to do so depends on  whether the child is a hardened key or not (or, equivalently, whether i ≥ 231), and whether we're talking about private or public keys.

This project implements two-party BIP32 hardened child key derivation. Given a parent extended key, chian code and an index i, it will output a new chain code and IL (used to compute the corresponding child extended key). For details on the key derivation process, see [bip-0032.mediawiki](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki#user-content-Child_key_derivation_CKD_functions).

## Tutorial

The project can be deployed, run and tested through the following steps.

1. Environment preparation
2. Project deployment
3. Test
4. Interface
5. Notice

Please refer to the [user manual](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/blob/master/docs/User%20Manual.md) for the specific content of the above steps.

## Technical principles

This project uses the Yao’s Garbled Circuit in [ABY framework](https://github.com/encryptogroup/ABY) to implement the 2PC HMAC512 algorithm and convert the results to Boolean shares. Use OT extension and  large number arithmetic to convert the Boolean shares to the Yao shares on large numbers.

This project uses the optimization solution of Yao’s Garbled Circuit，such as Point and Permute, Free XOR, Half Gates, Effificient Garbling from a Fixed-Key Blockcipher, OT extension and SIMD, which improves the calculation efficiency and reduces the communication overhead.

## Community discussion

Here is an open, equal and inclusive MPC enterprise community, where you can connect with other users and contributors, get help and participate. For any bugs or requests, you can [report](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/issues) on GitHub and get answers.

## References

[1] Demmler, Daniel, Thomas Schneider, and Michael Zohner. "ABY-A framework for efficient mixed-protocol secure two-party computation." *NDSS*. 2015.

[2] Bellare, Mihir, et al. "Efficient garbling from a fixed-key blockcipher." *2013 IEEE Symposium on Security and Privacy*. IEEE, 2013.

[3] Zahur, Samee, Mike Rosulek, and David Evans. "Two halves make a whole: Reducing data transfer in garbled circuits using half gates." *Advances in Cryptology-EUROCRYPT 2015: 34th Annual International Conference on the Theory and Applications of Cryptographic Techniques, Sofia, Bulgaria, April 26-30, 2015, Proceedings, Part II 34*. Springer Berlin Heidelberg, 2015.

[4] Kolesnikov, Vladimir, and Thomas Schneider. "Improved garbled circuit: Free XOR gates and applications." *Automata, Languages and Programming: 35th International Colloquium, ICALP 2008, Reykjavik, Iceland, July 7-11, 2008, Proceedings, Part II 35*. Springer Berlin Heidelberg, 2008.

[5] Beaver, Donald, Silvio Micali, and Phillip Rogaway. "The round complexity of secure protocols." *Proceedings of the twenty-second annual ACM symposium on Theory of computing*. 1990.

## License

[License](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/blob/master/LICENSE.txt)
