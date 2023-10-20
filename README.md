# [MPC_BIP32_Hardened_Child_Key](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/#MPC_BIP32_Hardened_Child_Key)

MPC，即“多方计算”，它是一种密码协议，使多个参与方能够共同进行计算一个特定的函数，同时保持这些参与方的输入保密。MPC是一种非常强大的技术，广泛应用于隐私保护数据分析、安全多方机器学习和隐私保护金融计算等领域。使用MPC计算交易签名时，多个参与方使用各自的私密碎片计算部分签名，并将这些签名合并成完整有效的签名。

比特币BIP-32规范详细定义了HD算法原理和各种推导规则。BIP39、BIP43、BIP44都是兼容BIP32的协议，后来还被比特币以外的区块链所借鉴，形成了区块链行业的共识。Child key derivation (CKD) functions指的是给出一个扩展密钥和索引，方法期望能够计算出对应的子扩展密钥。

本项目实现了两方BIP32硬化子密钥派生，输入链码（chain code）、扩展密钥分片以及子扩展密钥索引，可以生成新的链码以及用于计算子扩展密钥的IL。

## [教程](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/#教程)

通过下面的步骤可以对该项目进行部署、运行和测试。

1. 环境准备

2. 项目部署

3. 测试

5. 接口使用

6. 注意事项

以上步骤的具体操作请查阅[用户手册]()

## [技术原理](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/#技术原理)

该项目以论文**[ABY – A Framework for Effificient Mixed-Protocol Secure Two-Party Computation]**为技术支撑，使用ABY框架实现了布尔共享与姚氏共享的转换、利用姚氏电路实现两方的HMAC512运算，并使用OT扩展、大数运算等技术实现了大数的布尔共享与算数共享的转换。

该项目使用了姚氏共享的优化方案，采用了半门技术、Free XOR、固定密钥AES（构建混淆电路）等优化技术。

## [社区讨论](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/#社区讨论)

这里是一个开放、平等、包容的MPC企业社区，在这里你可以与其他用户和贡献者联系，可以获得帮助并参与其中。对于任何错误或请求，您可以在 GitHub 上[报告](https://github.com/antalpha-com/MPC_BIP32_Hardened_Child_Key/issues)并获得答案。







