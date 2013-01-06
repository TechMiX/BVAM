BVAM
====

The Bitcoin Vanity Address Manager

A software for generating ECDSA keypairs and merging split-keys used in vanity address pools. Also manages and saves your private keys in a secure manner with AES-256.

Idea
----

Addresses in Bitcoin system are too long to remember, but it is not always neccesary to express them completely. We can just use the first characters and they can be eaisly found in the system (see  [firstbits.com](http://firstbits.com)). Addresses with easy-to-remember first bits are called Vanity Addresses.

A bitcoin address is simply mapped from a public key. To make a vanity address, one should randomly generate keypairs and sees if the bitcoin address correspoding to them is desierable or not. This can be done with softwares like [VanityGen](http://github.com/samr7/vanitygen) or [Nyhm's Vanity Generator](http://silverthreadsoftware.com/bitcoin/vanity/). But obviously it needs a lot of computing power.

Based on an idea from [JoelKatz](https://bitcointalk.org/index.php?topic=81865.msg901491#msg901491) in bitcoin forum, two or more parties can help each other make vanity addresses without risking anything. 

Assume Bob has a lot of computing power and Alice wants to create a vanity address. Alice generates a key-pair. She keeps the private key and sends her public key plus her desired prefix to Bob. Bob generate a lot of random private keys, that we call Solutions, and merges them with Alice's public key and checks if the result makes a bitcoin address with the desired prefix. When Bob founds a Solution he sends it to Alice. Alice then merges the Solution with her private key and makes a new safe private key and enjoys it!

The pseudo-code in the bellow explains the same:

Alice:

+ AlicePrivateKey = RandomNumberGenerator()
+ AlicePublicKey = PublicKeyGenerator(AlicePrivateKey)
+ AliceDesiredPrefix = "1Alice"
+ Alice sends AlicePublicKey and AliceDesiredPrefix to Bob and waits for Solution.
+ Alice receives Solution.
+ AliceNewPrivateKey = MergeSplitKeys(AlicePrivateKey, Solution)
+ AliceNewPublicKey = PublicKeyGenerator(AliceNewPrivateKey)
+ AliceBitcoinAddress = BitcoinAddressGenerator(AliceNewPublicKey)

Bob:

+ Bob receives AlicePublicKey and AliceDesiredPrefix from Alice. 
+ While Solution is not found:
	+ Solution = RandomNumberGenerator()
	+ TempPublicKey = MergeSplitKeys(AlicePublicKey, Solution)
	+ If BitcoinAddressGenerator(TempPublicKey) starts with AliceDesiredPrefix:
		+ Bob sends Solution to Alice.

This idea works and is secure becuase of the fundamental structure that is being used in Bitcoin which is based on [Elliptic Curve Cyptography](http://en.wikipedia.org/wiki/Elliptic_curve_cryptography).

License
-------

This software is licensed under [GNU General Public License v3](http://www.gnu.org/licenses/gpl.txt).

It includes software developed by the [OpenSSL Project](http://www.openssl.org/) and cryptographic software written by Eric Young (eay@cryptsoft.com).

Donation
--------

[1BVAMa6ngNQozzdJVDC4WMEN579dRrqSj4](bitcoin:1BVAMa6ngNQozzdJVDC4WMEN579dRrqSj4)
