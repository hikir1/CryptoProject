a.out : hello.cpp
	g++ hello.cpp -D NDEBUG

atm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp client.cpp aes/**.cpp RSA/**.cpp \
		ssh.cpp hmac/**.cpp util.cpp -D NDEBUG -D ATM -o atm.out -lgmp

bank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp bank.cpp aes/**.cpp RSA/**.cpp \
		ssh.cpp hmac/**.cpp util.cpp -D NDEBUG -D BANK -o bank.out -lgmp

testatm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp client.cpp aes/**.cpp RSA/**.cpp \
			ssh.cpp hmac/**.cpp util.cpp -D NDEBUG -D ATM -o atm.out -lgmp -g

testbank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp bank.cpp aes/**.cpp RSA/**.cpp \
		ssh.cpp hmac/**.cpp util.cpp -D BANK -o bank.out -lgmp -g

tserver : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp bank.cpp aes/**.cpp RSA/**.cpp \
		ssh.cpp hmac/**.cpp util.cpp -D BANK -D NENCRYPT -o bank.out -lgmp -g
