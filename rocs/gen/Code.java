/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.  
 */
public class Code {

  /**
   * @param args
   */
  public static void main(String[] args) {
    if( args.length > 0 ) {
      String sEmail = args[0];
      System.out.println("Code="+code(sEmail));
    }
    else {
      System.out.println("Please provide an email address for generating a code.");
      System.out.println("java Code <email>");
    }
  }

  public static int code(String sEmail) {
    byte email[] = sEmail.toLowerCase().getBytes();
    byte x1 = 0;
    byte x2 = 0;
    for(int i = 0; i < email.length; i+=2 ) {
      x1 ^= email[i];
      if( i+1 < email.length )
        x2 ^= email[i+1];
    }
    int code = (x1 << 8) + x2;
    return code;
  }
}
