using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ChatHandler : MonoBehaviour
{
    public RDLPIM_Controller rdlController;
    public Text textbox;
    private string msg = "nothing recieved";
    private bool dirty = false;
    // Start is called before the first frame update
    void Start()
    {
        rdlController.ChatRecieved += onChatRecieved;
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

    protected void onChatRecieved(object source, string message)
    {
        msg = message;
        dirty = true;
    }

    private void OnDestroy()
    {
        rdlController.ChatRecieved -= onChatRecieved;
    }
}
