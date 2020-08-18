using System;
using UnityEngine;
using System.Text;
using System.Collections.Generic;

public class RDLPIM_Controller : MonoBehaviour
{
    private RDLPIM_Client RDL;

    public int Port = 8000;
    public string IP = "127.0.0.1";
    public byte[] recBuff;
    public int m_bytes;

    public delegate void InfoRecievedHandler(object source, string message);
    public event InfoRecievedHandler InfoRecieved;

    public delegate void ChatRecievedHandler(object source, string message);
    public event ChatRecievedHandler ChatRecieved;

    public delegate void DataRecievedHandler(object source, List<DataElement> data);
    public event DataRecievedHandler DataRecieved;

    // Start is called before the first frame update
    void Start()
    {
        RDL = RDLPIM_Client.GetInstance();

        //subscribe to data recieved event
        RDL.DataRecieved += onRDLPIMmessage;
        RDL.RDLPIM_ConnectionLost += onRDLPIM_Disconnect;

        //setup endpoint details
        RDL.SetIP(IP);
        RDL.SetPort(Port);

        //RDLPIM handshake
        RDL.InitiConnection();
    }

    // Update is called once per frame
    void Update()
    {
        RDL.Receive();
    }

    public void onRDLPIMmessage(object source, DataRecievedEventArgs e)
    {

        RDLPIM_FucntionCode functionCode;
        int bytes;

        functionCode = e.FucntionCode;
        bytes = e.bytes;
        m_bytes = bytes;

        if (functionCode == RDLPIM_FucntionCode.chat)
        {
            string msg = System.Text.Encoding.Default.GetString(e.Data);

            ChatRecieved(this, msg);
        }
        else if (functionCode == RDLPIM_FucntionCode.Info)
        {
            string msg = System.Text.Encoding.Default.GetString(e.Data);

            InfoRecieved(this, msg);
        }
        else if (functionCode == RDLPIM_FucntionCode.data)
        {
            Debug.Log("recieved Data");
            List<DataElement> data = DataElement.DeserialiseArray(e.Data);

            DataRecieved(this, data);
            
        } else
        {
            Debug.Log("Unhandled Function:" + functionCode + " recieved containing " + bytes + " of data.");
        }
    }

    public void onRDLPIM_Disconnect(object source, EventArgs e)
    {
        Debug.Log("Lost Connection to the RDLPIM");
    }
    byte[] Append(byte[] dst, byte[] src){

        byte[] ret = new byte[dst.Length + src.Length];

        for(int i = 0; i< dst.Length; i++)
        {
            ret[i] = dst[i];
        }

        for(int i=0; i< src.Length; i++)
        {
            ret[dst.Length + i] = src[i];
        }

        return ret;
    }

    public void SendChat(string message)
    {
        RDL.Send(header.Generate(RDLPIM_FucntionCode.chat, Encoding.ASCII.GetBytes(message)));
        
    }

    public void SendPull(List<DataElement> elements)
    {
        byte[] data = new byte[0];

        foreach(DataElement item in elements)
        {
            data = Append(data, item.SerialiseName());
        }

        RDL.Send(header.Generate(RDLPIM_FucntionCode.pull, data));

    }

    public void SendPush(List<DataElement> elements)
    {
        byte[] data = new byte[0];

        foreach (DataElement item in elements)
        {
            data = Append(data, item.Serialise());
        }

        RDL.Send(header.Generate(RDLPIM_FucntionCode.push, data));

    }

    public void SendSubscribe(List<DataElement> elements)
    {
        byte[] data = new byte[0];

        foreach (DataElement item in elements)
        {
            data = Append(data, item.SerialiseName());
        }

        RDL.Send(header.Generate(RDLPIM_FucntionCode.subscribe, data));

    }

    public void SendUnsubscribe(List<DataElement> elements)
    {
        byte[] data = new byte[0];

        foreach (DataElement item in elements)
        {
            data = Append(data, item.SerialiseName());
        }

        RDL.Send(header.Generate(RDLPIM_FucntionCode.unsubscribe, data));

    }



    public void RetryConnection()
    {
        RDL.RetryConnection();
    }

    private void OnDestroy()
    {
        RDL.DataRecieved -= onRDLPIMmessage;
    }
}
