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
        

        //Todo This could be multiple messages stacked up between unity frames, need to handle this.
        recBuff = e.Data;

        int functionCode;
        int bytes;

        functionCode = BitConverter.ToInt32(e.Data, 0);
        bytes = BitConverter.ToInt32(e.Data, 4);

        if(functionCode == 2 || functionCode == 8 )
        {
            Debug.Log("recieved Chat " + functionCode + " with " + bytes + " of data.");
            byte[] msgRaw = new byte[bytes];
            for(int i = 0; i < bytes; i++)
            {
                msgRaw[i] = e.Data[i + 8];
            }
            
            string msg = System.Text.Encoding.Default.GetString(msgRaw);
            Debug.Log(msg);
        } else
        {
            Debug.Log("recieved Function code " + functionCode + " with " + bytes + " of data.");
        }
    }

    private void OnDestroy()
    {
        RDL.DataRecieved -= onRDLPIMmessage;
    }
}
