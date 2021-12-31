import static util.RabinPrimalityTestUtils.pow;

import java.math.BigInteger;
import util.RSAUtils;
import util.RabinPrimalityTestUtils;

public class Main {

  public static void main(String[] args) {
    // lab 4
    System.out.println(RabinPrimalityTestUtils.getPrimeNumber(100));

    // lab 5
    // 0 - public key; 1 - secret key; 2 - P
    BigInteger[] keys = RSAUtils.evaluateKeys();
    BigInteger message = BigInteger.valueOf(3);
    System.out.println("Secret message = " + message);
    BigInteger E = pow(message, keys[0]).mod(keys[2]);
    System.out.println("E = " + E);
    BigInteger D = pow(E, keys[1]).mod(keys[2]);
    System.out.println("D = " + D);
  }
}
