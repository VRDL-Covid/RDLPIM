
using UnityEngine;
using UnityEngine.UI;

public class ChatSender : MonoBehaviour
{
    
    public InputField source;

    public void Send()
    {
        RDLPIM_Controller rdlController = RDLPIM_Controller.GetInstance();
        rdlController.SendChat(source.text);
    }
}
