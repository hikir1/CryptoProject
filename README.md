# Hello fellas

Group 11 White Hat Project

Dhruv Patel, Richard Pawełkiewicz and Nathan Whitney

Our code is meant to be compiled on a machine running on a Linux OS,
specifically, Ubuntu. You can compile our code by running `make` in the unzipped
project directory. Two executibles will be created, `bank.out` and `atm.out`.
They both take two arguments: The HOST and the PORT. Assuming you are testing both on the
same machine, you can use `localhost` as the host, and any unused port in the range of
1024-65535. You may need to try multiple different ports, depending on which ports are actively
being used. These arguments specify the host and port on which the bank should
listen and the atm should try to connect.
Run the bank first, then run the atm. Follow the instructions given. Enjoy :).

Oh! One more note. Our code requires GMP, the GNU MultiPrecision library to be installed.
You can probably do this most easily through your package manager: `sudo apt install libgmp-dev`.
We use version 10, in case it matters.

### We are tired. Time to sleep...
