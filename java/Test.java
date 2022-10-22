
public class Test
{
    public static void main(String argv[])
    {
        System.loadLibrary("javaMtMaths"); // Attempts to load example.dll (on Windows) or libexample.so (on Linux)
        System.out.println(MtMathC.foo());
    }
}