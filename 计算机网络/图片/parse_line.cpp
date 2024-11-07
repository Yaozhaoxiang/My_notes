//从状态机，用于分析出一行内容
//返回值为行的读取状态，有LINE_OK,LINE_BAD,LINE_OPEN
http_conn::LINE_STATUS http_conn::parse_line()
{
    char temp;
    /* m_checked_idx指向buffer中当前正在分析的字节，m_read_idx指向buffer中客户数据的尾部下一个字节。
    buffer中0~m_checked_idx都已经被分析完毕，第m_checked_idx~(m_read_idx-1)字节由下面遍历 */

    for (; m_checked_idx < m_read_idx; ++m_checked_idx)
    {
        /* 获取当前要分析的字节 */
        temp = m_read_buf[m_checked_idx];
        /* 如果当前的字节是 \r 则说明可能读取到一个完整的行 */
        if (temp == '\r')
        {
            /* 如果 \r 字符恰好是目前m_read_buf中的最后一个已经被读入的客户数据，那么这次分析没有读取到一个完整的行，
            返回 LINE_OPEN 一表示还需要继续读取客户端数据 */
            if ((m_checked_idx + 1) == m_read_idx)
                return LINE_OPEN;
            /* 如果下一个是 \n 则说明读取到一个完整的行 */
            else if (m_read_buf[m_checked_idx + 1] == '\n')
            {
                m_read_buf[m_checked_idx++] = '\0';
                m_read_buf[m_checked_idx++] = '\0';
                return LINE_OK;
            }
            /* 否则，说明客户发送的 HTTP 请求语法有问题 */
            return LINE_BAD;
        }
        /* 如果当前字节是 \n ，则说明可能读取到一个完整的行 */
        else if (temp == '\n')
        {
            if (m_checked_idx > 1 && m_read_buf[m_checked_idx - 1] == '\r')
            {
                m_read_buf[m_checked_idx - 1] = '\0';
                m_read_buf[m_checked_idx++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    /* 如果所有的内容都分析完也没有遇到 \r ，则说明还需要继续读取 */
    return LINE_OPEN;
}