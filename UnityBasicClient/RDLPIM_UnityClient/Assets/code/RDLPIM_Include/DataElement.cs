using System;
using System.Collections.Generic;
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
        it++;
        offset = it;
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
        it++;
        offset += it;
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

    public DataElement()
    {
        
    }


    public string GetName() { return m_varname; }
    public string GetType() { return m_DataType; }

    public byte[] GetData() { return m_data; }
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


    public void Set(bool value)
    {
        m_DataType = "bool";
        m_bytes = sizeof(bool);
        m_data = BitConverter.GetBytes(value);
    }

    public void Set(int value)
    {
        m_DataType = "int";
        m_bytes = sizeof(int);
        m_data = BitConverter.GetBytes(value);
    }

    public void Set(float value)
    {
        m_DataType = "float";
        m_bytes = sizeof(float);
        m_data = BitConverter.GetBytes(value);
    }

    public void Set(double value)
    {
        m_DataType = "double";
        m_bytes = sizeof(double);
        m_data = BitConverter.GetBytes(value);
    }

    public void Set(byte[] value)
    {
        m_DataType = "blob";
        m_bytes = value.Length;
        m_data = value;
    }


    public byte[] Serialise()
    {
        byte[] top = new byte[1];
        byte[] tail = new byte[1];
        byte[] equ = new byte[1];

        top[0] = (byte)'{';
        tail[0] = (byte)'}';
        equ[0] = (byte)'=';
        
        byte[] ret = new byte[0];
        
        ret = Append(ret, top);
        ret = Append(ret, Encoding.ASCII.GetBytes(m_varname));
        ret = Append(ret, equ);
        ret = Append(ret, Encoding.ASCII.GetBytes(m_DataType));
        ret = Append(ret, equ);
        ret = Append(ret, BitConverter.GetBytes(m_bytes));
        ret = Append(ret, GetData());
        ret = Append(ret, tail);

        return ret;
    }

    public static List<DataElement> DeserialiseArray(byte[] serialised)
    {
        List<DataElement> ret = new List<DataElement>();

        int nextStart;
        int nextEnd = -1;
        int sizeOfNext = 0;
        byte[] temp;
        while (nextEnd <= serialised.Length)
        {
            //get next starting positino (last end +1
            nextStart = nextEnd + 1;
            
            //if outside the serialised data then finished so break loop
            if (nextStart >= serialised.Length)
                break;

            //serach for next packet incase of padding
            while ((serialised[nextStart] != '{') && nextStart <= serialised.Length)
            {
                nextStart++;
            }

            //next end has to be after then next start
            nextEnd = nextStart+1;

            //hunt for end of packet
            while ((serialised[nextEnd] != '}') && nextEnd <= serialised.Length)
            {
                nextEnd++;
            }

            //allocate memory for chunk
            sizeOfNext = (nextEnd - nextStart) + 1;
            temp = new byte[sizeOfNext];

            //copy chunk
            for(int i = nextStart; i<(nextStart+sizeOfNext); i++)
            {
                temp[i-nextStart] = serialised[i];
            }

            DataElement element = new DataElement();

            element.Deserialise(temp);

            ret.Add(element);
        }

        return ret;
    }


    byte[] Append(byte[] dst, byte[] src)
    {

        byte[] ret = new byte[dst.Length + src.Length];

        for (int i = 0; i < dst.Length; i++)
        {
            ret[i] = dst[i];
        }

        for (int i = 0; i < src.Length; i++)
        {
            ret[dst.Length + i] = src[i];
        }

        return ret;
    }
}
