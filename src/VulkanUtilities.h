struct QueueFamilyIndices{
    int graphicsFamily = -1;
    bool isValid()
    {
        return graphicsFamily >= 0;

    }
};