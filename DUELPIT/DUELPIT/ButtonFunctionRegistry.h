#pragma once

class ButtonFunctionRegistry : public Singleton<ButtonFunctionRegistry>
{
    friend class Singleton<ButtonFunctionRegistry>;

private:
    std::unordered_map<std::string, std::function<void(const Json&)>> m_functions;

private:
    ButtonFunctionRegistry();
    virtual ~ButtonFunctionRegistry() = default;

public:
    void RegisterFunction(const std::string& name, const std::function<void(const Json&)>& function);

    std::function<void(const Json&)> GetFunction(const std::string& name) const;
};