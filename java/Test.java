
public class Test
{
    public static void main(String argv[])
    {
        System.loadLibrary("javaMtMaths"); // Attempts to load example.dll (on Windows) or libexample.so (on Linux)
        System.out.println(MtMathC.foo());
        var maths = new MtMath_BigInt();
        MtMathC.init_big_int(maths);
        System.out.println(maths.getFlags());
    }
}