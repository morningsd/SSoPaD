package util;

import java.math.BigInteger;
import java.util.Random;

public class RabinPrimalityTestUtils {

  private static final int[] smallPrimeNumbers = {2, 3, 5, 7, 11, 13};

  public static BigInteger getPrimeNumber(long PMax) {
    BigInteger a = BigInteger.valueOf(getRandomSmallPrimeNumber());
    BigInteger K = BigInteger.valueOf(Math.round(customLog(a.doubleValue(), (double) PMax / 2)));
    BigInteger P1 = pow(a, K).multiply(BigInteger.TWO).add(BigInteger.ONE);
    BigInteger P2 = pow(a, K).multiply(BigInteger.TWO).subtract(BigInteger.ONE);
    while (true) {
      if (isPrime(a, P1)) return P1;
      if (isPrime(a, P2)) return P2;
      P1 = P1.add(BigInteger.TWO);
      P2 = P2.subtract(BigInteger.TWO);
    }
  }

  private static int getRandomSmallPrimeNumber() {
    return smallPrimeNumbers[new Random().nextInt(smallPrimeNumbers.length)];
  }

  public static double customLog(double base, double logNumber) {
    return Math.log(logNumber) / Math.log(base);
  }

  private static boolean isPrime(BigInteger a, BigInteger P) {
    BigInteger powResult = pow(a, P.subtract(BigInteger.ONE));
    BigInteger result = powResult.mod(P);
    return result.equals(BigInteger.ONE);
  }

  public static BigInteger pow(BigInteger base, BigInteger exponent) {
    BigInteger result = BigInteger.ONE;
    while (exponent.signum() > 0) {
      if (exponent.testBit(0)) result = result.multiply(base);
      base = base.multiply(base);
      exponent = exponent.shiftRight(1);
    }
    return result;
  }

}
