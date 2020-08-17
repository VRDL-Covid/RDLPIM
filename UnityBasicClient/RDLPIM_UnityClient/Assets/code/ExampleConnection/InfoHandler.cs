using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InfoHandler : MonoBehaviour
{
    public RDLPIM_Controller rdlController;
    public Text textbox;
    private string msg = "nothing recieved";
    private bool dirty = false;
    // Start is called before the first frame update
    void Start()
    {
        rdlController.InfoRecieved += onRawTextRecieved;
    }

    // Update is called once per frame
    void Update()
    {
        if (dirty)
        {
            textbox.text = msg;
            dirty = false;
        }
        
    }

    protected void onRawTextRecieved(object source, string message)
    {
        msg = message;
        dirty = true;
    }

    private void OnDestroy()
    {
        rdlController.InfoRecieved -= onRawTextRecieved;
    }
}
