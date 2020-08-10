atm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ **.cpp **/**.cpp -D NDEBUG -D ATM -o atm.out

bank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ **.cpp **/**.cpp -D NDEBUG -D BANK -o bank.out

testatm : **.[ch]pp **/**.[ch]pp **/**.h
	g++ **.cpp **/**.cpp -D ATM -o atm.out

testbank : **.[ch]pp **/**.[ch]pp **/**.h
	g++ **.cpp **/**.cpp -D BANK -o bank.out
