using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.UIElements;

public class RDLPIM_Controller : MonoBehaviour
{
    private RDLPIM_Client RDL;
    
    public int Port = 8000;
    public string IP = "127.0.0.1";
    public byte[] recBuff;
    public int m_bytes;

    public delegate void ChatRecievedHandler(object source, string message);
    public event ChatRecievedHandler ChatRecieved;

    public delegate void InfoRecievedHandler(object source, string message);
    public event InfoRecievedHandler InfoRecieved;

    public delegate void DataRecievedHandler(object source, byte[] data);
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
        RDL.InitiConnection();
        RDL.Receive();
    }

    public void onRDLPIMmessage(object source, DataRecievedEventArgs e)
    {
       
        RDLPIM_FucntionCode functionCode;
        int bytes;

        functionCode = e.FucntionCode;
        bytes = e.bytes;
        m_bytes = bytes;

        if(functionCode == RDLPIM_FucntionCode.chat  )
        {
            string msg = System.Text.Encoding.Default.GetString(e.Data);

            ChatRecieved(this, msg);
        } 
        else if (functionCode == RDLPIM_FucntionCode.Info)
        {
            string msg = System.Text.Encoding.Default.GetString(e.Data);

            InfoRecieved(this, msg);
        }       
        else if(functionCode == RDLPIM_FucntionCode.data) 
        {
            DataRecieved(this, e.Data);
        } else
        {
            Debug.Log("recieved Function:" + functionCode + " with " + bytes + " of data.");
        }
    }

    public void onRDLPIM_Disconnect(object source, EventArgs e)
    {
        Debug.Log("Lost Connection to the RDLPIM");
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
