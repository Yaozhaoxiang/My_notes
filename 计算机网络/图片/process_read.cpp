/* 主状态机 */
http_conn::HTTP_CODE http_conn::process_read()
{
    LINE_STATUS line_status = LINE_OK; /* 记录当前行读取的状态 */
    HTTP_CODE ret = NO_REQUEST; /* 记录HTTP请求的处理结果 */
    char *text = 0;


    /* 主状态机，用于从buffer中取出完整的行 
    这里为什么要写两个判断条件？第一个判断条件为什么这样写？*/
    while ((m_check_state == CHECK_STATE_CONTENT && line_status == LINE_OK) || ((line_status = parse_line()) == LINE_OK))
    {
        text = get_line(); /* 当前起始位置 */
        m_start_line = m_checked_idx; /* 记录下一行的起始位置 */
        LOG_INFO("%s", text);

        //主状态机的三种状态转移逻辑
        switch (m_check_state)
        {
        case CHECK_STATE_REQUESTLINE:
        {
            //解析请求行
            ret = parse_request_line(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            break;
        }
        case CHECK_STATE_HEADER:
        {
            //解析请求头
            ret = parse_headers(text);
            if (ret == BAD_REQUEST)
                return BAD_REQUEST;
            else if (ret == GET_REQUEST) //完整解析GET请求后，跳转到报文响应函数
            {
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT:
        {
            //解析消息体
            ret = parse_content(text);

            //完整解析POST请求后，跳转到报文响应函数
            if (ret == GET_REQUEST)
                return do_request();
            
            //解析完消息体即完成报文解析，避免再次进入循环，更新line_status
            line_status = LINE_OPEN;
            break;
        }
        default:
            return INTERNAL_ERROR;
        }
    }
    return NO_REQUEST;
}