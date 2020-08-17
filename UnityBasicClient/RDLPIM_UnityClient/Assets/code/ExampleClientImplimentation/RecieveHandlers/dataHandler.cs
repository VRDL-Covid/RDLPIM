using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class dataHandler : MonoBehaviour
{
    public static List<dataHandler> dataHandlers = new List<dataHandler>();

    public RDLPIM_Controller rdlController;
    public InputField varName;
    public InputField value;

    dataHandler()
    {
        dataHandlers.Add(this);
    }

    ~dataHandler()
    {
        dataHandlers.Remove(this);
    }

    void start()
    {

    }

    void Update()
    {

    }

    public string GetVarname() { return varName.text; }

    public void SendPulls()
    {
        List<DataElement> reqs = new List<DataElement>();

        foreach(dataHandler item in dataHandlers)
        {
            if(item.GetVarname() != "")
            {
                reqs.Add(new DataElement(item.GetVarname()));
            }
        }

        rdlController.SendPull(reqs);
    }
}
