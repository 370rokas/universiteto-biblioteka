group "default" {
    targets = ["ub-frontend", "ub-backend"]
}

target "ub-frontend" {
    context = "./frontend"
    dockerfile = "dockerfile"
    tags = ["370rokas/ub-frontend:latest"]
}

target "ub-backend" {
    context = "./backend"
    dockerfile = "dockerfile"
    tags = ["370rokas/ub-backend:latest"]
}

