package util;

import static util.RabinPrimalityTestUtils.customLog;

import java.math.BigDecimal;
import java.math.BigInteger;

public class RSAUtils {

  public static BigInteger getPublicKey(BigInteger P, BigInteger FE) {
    BigInteger start = BigInteger.valueOf(Math.round(customLog(2.0, P.doubleValue())));
    BigInteger Ko = start.add(BigInteger.ONE);
    while (true) {
      if (gcd(Ko, FE).equals(BigInteger.ONE)) {
        return Ko;
      }
      Ko = Ko.add(BigInteger.ONE);
    }
  }

  public static BigInteger getSecretKey(BigInteger Ko, BigInteger Fe) {
    long z = 1;
    while (true) {
      BigDecimal[] tmp = new BigDecimal(Fe.multiply(BigInteger.valueOf(z)).add(BigInteger.ONE)).divideAndRemainder(new BigDecimal(Ko));
      if (tmp[1].equals(BigDecimal.ZERO)) {
        return tmp[0].toBigInteger();
      }
      ++z;
    }
  }

  public static BigInteger[] evaluateKeys() {
    BigInteger[] result = new BigInteger[3];
    BigInteger p = RabinPrimalityTestUtils.getPrimeNumber(30);
    BigInteger q = RabinPrimalityTestUtils.getPrimeNumber(30);
    BigInteger P = p.multiply(q);
    BigInteger FE = eulerFunction(p, q);
    result[0] = getPublicKey(P, FE);
    result[1] = getSecretKey(result[0], FE);
    result[2] = P;

    return result;
  }

  private static BigInteger eulerFunction(BigInteger p, BigInteger q) {
    return p
        .subtract(BigInteger.ONE)
        .multiply(q.subtract(BigInteger.ONE));
  }

  private static BigInteger gcd(BigInteger a, BigInteger b) {
    if (a.equals(BigInteger.ZERO))
      return b;
    return gcd(b.mod(a), a);
  }

}
