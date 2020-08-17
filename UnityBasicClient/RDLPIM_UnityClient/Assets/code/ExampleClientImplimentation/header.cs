
using System;

public class header
{
    public static byte[] Generate(RDLPIM_FucntionCode function, byte[] data)
    {
        byte[] cap = new byte[8+data.Length];

        byte[] f = BitConverter.GetBytes((int)function);
        byte[] s = BitConverter.GetBytes(data.Length);

        for (int i = 0; i<4; i++)
        {
            cap[i] = f[i];
        }

        for (int i = 0; i < 4; i++)
        {
            cap[i+4] = s[i];
        }

        for(int i = 0; i< data.Length; i++)
        {
            cap[i + 8] = data[i];
        }


        return cap;
    } 
}
