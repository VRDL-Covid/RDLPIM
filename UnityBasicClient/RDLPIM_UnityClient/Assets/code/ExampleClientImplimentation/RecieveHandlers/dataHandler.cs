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
    public string valueMSG;

    dataHandler()
    {
        dataHandlers.Add(this);
    }

    ~dataHandler()
    {
        dataHandlers.Remove(this);
    }

    public void Start()
    {
        rdlController.DataRecieved += onNewData;
    }

    public void Update()
    {
        value.text = valueMSG;
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

    protected void onNewData(object source, List<DataElement> newData)
    {
        foreach(DataElement element in newData)
        {
            if(element.GetName() == varName.text)
            {
                if(element.GetType() == "int")
                {
                    valueMSG = BitConverter.ToInt32(element.GetData(),0).ToString();
                }

                if (element.GetType() == "double")
                {
                    valueMSG = BitConverter.ToInt32(element.GetData(), 0).ToString();
                }

                if (element.GetType() == "bool")
                {
                    valueMSG = BitConverter.ToBoolean(element.GetData(), 0).ToString();
                }

                if (element.GetType() == "float")
                {
                    valueMSG = BitConverter.ToSingle(element.GetData(), 0).ToString();
                }

                if (element.GetType() == "ERR-NFND")
                {
                    valueMSG = "ERR-NFND";
                }
            }
        }
    }

    private void OnDestroy()
    {
        rdlController.DataRecieved -= onNewData;
    }
}
