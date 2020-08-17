
using UnityEngine;
using UnityEngine.UI;

public class ChatSender : MonoBehaviour
{
    public RDLPIM_Controller rdlController;
    public InputField source;

    public void Send()
    {
        rdlController.SendChat(source.text);
    }
}
