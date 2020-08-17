using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class ConnectionDetails
{
    private byte[] m_rawPacket = new byte[1024];

    private int m_port;
    private int m_id;

    public int GetPort() { return m_port;}
    public int GetID() { return m_id; }

    public void Deserialise(byte[] rawPacket)
    {
        m_rawPacket = rawPacket;
        byte[] idData = new byte[4];
        byte[] portData = new byte[4];


        int i = 0;

        while (m_rawPacket[i] != '=')
        {
            i++;
        }
        i++;

        for (int j = 0; j < 4; j++)
        {
            portData[j] = m_rawPacket[j + i];
        }

        i += 5;

        for (int j = 0; j < 4; j++)
        {
            idData[j] = m_rawPacket[j + i];
        }


        m_id = BitConverter.ToInt32(idData, 0);
        m_port = BitConverter.ToInt32(portData, 0);
    }

}
