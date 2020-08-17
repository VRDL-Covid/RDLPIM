using System;
using System.Text;

public class DataElement
{
    private string m_varname;
    private string m_DataType;
    private int m_bytes;
    byte[] m_data;

    public void Deserialise(byte[] serialised)
    {
        //size of serialised packet
        int size = serialised.Length;

        //temp storage
        byte[] varname;
        byte[] dataType;
        byte[] sizeRaw = new byte[4];
        int offset = 0;
        int it = 0;

        #region parse variable name
        //get varname length
        while (serialised[offset+it] != '=')
        {
            it++;
        }

        //init varname bytearry
        varname = new byte[it-1];

        //copy varname data
        for(int j = 0; j< it - 1; j++)
        {
            varname[j] = serialised[j + 1];
        }
        offset = it++;
        it = 0;

        m_varname = System.Text.Encoding.Default.GetString(varname);
        #endregion

        #region parse data type
        //get dataType length
        while (serialised[offset + it] != '=')
        {
            it++;
        }
        dataType = new byte[it];

        //copy dataType data
        for (int j = 0; j < it ; j++)
        {
            dataType[j] = serialised[j + offset];
        }
        offset = it++;
        it = 0;

        m_DataType = System.Text.Encoding.Default.GetString(dataType);
        #endregion

        #region parse m_bytes
        for (int j=0; j<4; j++)
        {
            sizeRaw[j] = serialised[j + offset];
        }
        offset += 4;
        m_bytes = BitConverter.ToInt32(sizeRaw, 0);
        #endregion

        #region parse data
        if (m_bytes > 0)
        {
            m_data = new byte[m_bytes];

            for(int j=0; j<m_bytes; j++)
            {
                m_data[j] = serialised[j + offset];
            }
        }
        #endregion
    }

    public DataElement(string Varname)
    {
        m_varname = Varname;
    }

    public byte[] SerialiseName()
    {
        byte[] varname = Encoding.ASCII.GetBytes(m_varname) ;
        byte[] ret = new byte[varname.Length + 2];

        ret[0] = (byte)'{';

        for(int i = 0; i< varname.Length; i++)
        {
            ret[i + 1] = varname[i];
        }

        ret[ret.Length - 1] = (byte)'}';

        return ret;

    }


    public byte[] Serialise()
    {
        int size = m_varname.Length + m_DataType.Length + 4 + m_bytes + 2;
        byte[] serialised = new byte[size];

        serialised[0] = (byte)'{';




        return serialised;
    }

}
