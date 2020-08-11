using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor.Experimental.GraphView;
using UnityEngine;

public class RDLPIM_Controller : MonoBehaviour
{
    private RDLPIM_Client RDL;
    
    public int Port = 8000;
    public string IP = "127.0.0.1";
    public byte[] recBuff;
    public int m_bytes;

    // Start is called before the first frame update
    void Start()
    {
        RDL = RDLPIM_Client.GetInstance();

        //subscribe to data recieved event
        RDL.DataRecieved += onRDLPIMmessage;

        //setup endpoint details
        RDL.SetIP(IP);
        RDL.SetPort(Port);

        //RDLPIM handshake
        RDLPIM_Client.GetInstance().InitiConnection();
    }

    // Update is called once per frame
    void Update()
    {
        RDLPIM_Client.GetInstance().Receive();
    }

    public void onRDLPIMmessage(object source, DataRecievedEventArgs e)
    {
       
        RDLPIM_FucntionCode functionCode;
        int bytes;

        functionCode = e.FucntionCode;
        bytes = e.bytes;
        m_bytes = bytes;

        if(functionCode == RDLPIM_FucntionCode.chat || functionCode == RDLPIM_FucntionCode.Info )
        {
            Debug.Log("recieved Function:" + functionCode + " with " + bytes + " of data.");
            string msg = System.Text.Encoding.Default.GetString(e.Data);
            Debug.Log(msg);
        } else
        {
            Debug.Log("recieved Function:"+ functionCode + " with " + bytes + " of data.");
        }
    }

    private void OnDestroy()
    {
        RDL.DataRecieved -= onRDLPIMmessage;
    }
}
