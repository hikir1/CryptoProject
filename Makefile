atm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp client.cpp aes/**.cpp RSA/**.cpp \
		hmac/**.cpp util.cpp -D NDEBUG -D ATM -o atm.out -lgmp

bank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp bank.cpp aes/**.cpp RSA/**.cpp \
		hmac/**.cpp util.cpp -D NDEBUG -D BANK -o atm.out -lgmp

testatm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp client.cpp aes/**.cpp RSA/**.cpp \
			hmac/**.cpp util.cpp -D NDEBUG -D ATM -o atm.out -lgmp -g

testbank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ KeyGeneration.cpp KeyEncryption.cpp bank.cpp aes/**.cpp RSA/**.cpp \
		hmac/**.cpp util.cpp -D BANK -o atm.out -lgmp -g
